/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "filltool.h"

struct FillTool::Private
{
    QMap<QString, TAction *> actions;
    TupGraphicsScene *scene;
    QCursor insideCursor;
    QCursor contourCursor;
};

FillTool::FillTool() : k(new Private)
{
    setupActions();
}

FillTool::~FillTool()
{
}

void FillTool::init(TupGraphicsScene *scene)
{
    /*
    foreach (QGraphicsItem *item, scene->items()) {
             if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                 if (item->zValue() >= 20000 && item->toolTip().length()==0) {
                     // item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
                 } else {
                     item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                     item->setFlag(QGraphicsItem::ItemIsFocusable, false);
                 }
             } else {
                 item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
             }
    }
    */

    k->scene = scene;
}

QStringList FillTool::keys() const
{
    return QStringList() << tr("Internal fill") << tr("Line fill");
}

void FillTool::setupActions()
{
    TAction *action1 = new TAction(QIcon(kAppProp->themeDir() + "icons/inside.png"), tr("Internal fill"), this);
    action1->setShortcut(QKeySequence(tr("I")));
    action1->setToolTip(tr("Internal fill") + " - " + "I");
    k->insideCursor = QCursor(kAppProp->themeDir() + "cursors/paint.png");
    action1->setCursor(k->insideCursor);
    k->actions.insert(tr("Internal fill"), action1);
    
    TAction *action2 = new TAction(QIcon(kAppProp->themeDir() + "icons/contour.png"), tr("Line fill"), this);
    action2->setShortcut(QKeySequence(tr("B")));
    action2->setToolTip(tr("Line fill") + " - " + "B");
    k->contourCursor = QCursor(kAppProp->themeDir() + "cursors/contour_fill.png");
    action2->setCursor(k->contourCursor);
    k->actions.insert(tr("Line fill"), action2);
}

void FillTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    if (input->buttons() == Qt::LeftButton) {
        // SQA: Enhance this plugin to support several items with one click 
        // QList<QGraphicsItem *> list = scene->items(input->pos(), Qt::IntersectsItemShape, Qt::DescendingOrder, QTransform());

        QGraphicsItem *item = scene->itemAt(input->pos(), QTransform());

        if (!item) {
            #ifdef K_DEBUG
                QString msg = "FillTool::press() - No item found";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return;
        } else {
            if (TupGraphicLibraryItem *libraryItem = qgraphicsitem_cast<TupGraphicLibraryItem *>(item)) {
                if (libraryItem->type() == TupLibraryObject::Image) {
                    #ifdef K_DEBUG
                        QString msg = "FillTool::press() - Warning: item is a RASTER object!";
                        #ifdef Q_OS_WIN32
                            qWarning() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                    return;
                }
            } else {
                TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
                if (svg) {
                    #ifdef K_DEBUG
                        QString msg = "FillTool::press() - Warning: item is a SVG object!";
                        #ifdef Q_OS_WIN32
                            qWarning() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                    return;
                }

                if (item->zValue() < 20000 && scene->spaceMode() == TupProject::FRAMES_EDITION) {
                    #ifdef K_DEBUG
                        QString msg = "FillTool::press() - Warning: Object belongs to the background frames";
                        #ifdef Q_OS_WIN32
                            qWarning() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                    return;
                }
            }
        }

        if (qgraphicsitem_cast<TupItemGroup *>(item)) {
            TOsd::self()->display(tr("Error"), tr("Sorry, Groups can't be filled yet"), TOsd::Error);
            return;
        }
        
        if (QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(item)) {
            int position = -1;

            if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                position = scene->currentFrame()->indexOf(shape);
            } else {
                TupBackground *bg = scene->scene()->background();
                if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                    TupFrame *frame = bg->staticFrame();
                    position = frame->indexOf(shape);
                } else if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           TupFrame *frame = bg->dynamicFrame();
                           position = frame->indexOf(shape);
                } else {
                    #ifdef K_DEBUG
                        QString msg = "FillTool::press() - Fatal Error: Invalid spaceMode!"; 
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif 
                    return;
                }
            }
                
            if (position >= 0) {
                if (name() == tr("Internal fill")) {
                    shape->setBrush(brushManager->pen().brush());
                } else if (name() == tr("Line fill")) {
                           QPen pen = shape->pen();
                           pen.setBrush(brushManager->pen().brush());
                           shape->setPen(pen);
                }
                    
                QDomDocument doc;
                doc.appendChild(TupSerializer::properties(shape, doc));
                    
                TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                          scene->currentSceneIndex(), scene->currentLayerIndex(),
                          scene->currentFrameIndex(), position, QPointF(), 
                          scene->spaceMode(), TupLibraryObject::Item, 
                          TupProjectRequest::Transform, doc.toString());

                emit requested(&event);
            } else {
                #ifdef K_DEBUG
                    QString msg = "FillTool::press() - Fatal Error: Invalid object index [ " + QString::number(position) + " ]";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        } else {
            #ifdef K_DEBUG
                QString msg = "FillTool::press() - Fatal Error: QAbstractGraphicsShapeItem cast has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    }
}

void FillTool::move(const TupInputDeviceInformation *, TupBrushManager *, TupGraphicsScene *)
{
}

void FillTool::release(const TupInputDeviceInformation *, TupBrushManager *, TupGraphicsScene *)
{
}

QMap<QString, TAction *> FillTool::actions() const
{
    return k->actions;
}

int FillTool::toolType() const
{
    return TupToolInterface::Fill;
}
        
QWidget *FillTool::configurator()
{
    return 0;
}

void FillTool::aboutToChangeScene(TupGraphicsScene *)
{
}

void FillTool::aboutToChangeTool() 
{
    foreach (QGraphicsItem *item, k->scene->items()) {
             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
             item->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}

QPainterPath FillTool::mapPath(const QPainterPath &path, const QPointF &pos)
{
    QMatrix transform;
    transform.translate(pos.x(), pos.y());
    
    QPainterPath painter = transform.map(path);
    painter.closeSubpath();
    
    return painter;
}

QPainterPath FillTool::mapPath(const QGraphicsPathItem *item)
{
    return mapPath(item->path(), item->pos());
}

void FillTool::saveConfig()
{
}

void FillTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    // } else if (event->modifiers() != Qt::ShiftModifier && event->modifiers() != Qt::ControlModifier) {
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor FillTool::cursor() const
{
    if (name() == tr("Internal fill")) {
        return k->insideCursor;
    } else if (name() == tr("Line fill")) {
               return k->contourCursor;
    }

    return QCursor(Qt::ArrowCursor);
}
