/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
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

#include "tweener.h"
#include "configurator.h"
#include "tglobal.h"
#include "tdebug.h"
#include "taction.h"
#include "tnodegroup.h"
#include "tosd.h"
#include "tupsvg2qt.h"
#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicsscene.h"
#include "tupgraphicobject.h"
#include "tuppathitem.h"
#include "tupellipseitem.h"
#include "tuprectitem.h"
#include "tuplineitem.h"
#include "tupsvgitem.h"
#include "tupitemtweener.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tupscene.h"
#include "tuplayer.h"

#include <QPointF>
#include <QKeySequence>
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMatrix>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QDomDocument>
#include <QDir>

struct Tweener::Private
{
    QMap<QString, TAction *> actions;
    Configurator *configurator;

    TupGraphicsScene *scene;
    QGraphicsPathItem *path;
    QList<QGraphicsItem *> objects;

    TupItemTweener *currentTween;
    TNodeGroup *nodesGroup;

    bool isPathInScene;
    int initFrame;
    int initLayer;
    int initScene;

    TupToolPlugin::Mode mode;
    TupToolPlugin::EditMode editMode;

    QPointF itemObjectReference;
    QPointF pathOffset;
    QPointF firstNode;

    int baseZValue;
};

Tweener::Tweener() : TupToolPlugin(), k(new Private)
{
    setupActions();

    k->configurator = 0;
    k->path = 0;
    k->nodesGroup = 0;
    k->currentTween = 0;

    k->initFrame = 0;
    k->initLayer = 0;
    k->initScene = 0;
}

Tweener::~Tweener()
{
    delete k;
}

/* This method initialize the context of the plugin */

void Tweener::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    delete k->path;
    k->path = 0;
    delete k->nodesGroup;
    k->nodesGroup = 0;
    k->scene = scene;
    k->objects.clear();
    k->baseZValue = 20000 + (scene->scene()->layersTotal() * 10000);

    k->isPathInScene = false;
    k->pathOffset = QPointF(0, 0); 
    k->firstNode = QPointF(0, 0);
    k->itemObjectReference = QPointF(0, 0);

    k->mode = TupToolPlugin::View;
    k->editMode = TupToolPlugin::None;

    k->configurator->resetUI();

    QList<QString> tweenList = k->scene->scene()->getTweenNames(TupItemTweener::Position);
    if (tweenList.size() > 0) {
        k->configurator->loadTweenList(tweenList);
        setCurrentTween(tweenList.at(0));
    }

    int total = framesTotal();
    k->configurator->initStartCombo(total, k->scene->currentFrameIndex());
}

void Tweener::updateStartFrame(int index)
{
    if (k->initFrame != index && index >= 0) 
        k->initFrame = index;
}

/* This method returns the plugin name */

QStringList Tweener::keys() const
{
    return QStringList() << tr("Position Tween");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void Tweener::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    Q_UNUSED(brushManager);
    Q_UNUSED(scene);

    if (k->editMode == TupToolPlugin::Path && k->scene->currentFrameIndex() == k->initFrame) {
        if (k->path) {
            QPointF point = k->path->mapFromParent(input->pos());
            QPainterPath path = k->path->path();
            path.cubicTo(point, point, point);
            k->path->setPath(path);
        }
    } 
}

/* This method is executed while the mouse is pressed and on movement */

void Tweener::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method finishes the action started on the press method depending
 * on the active mode: Selecting an object or Creating a path
*/

void Tweener::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (scene->currentFrameIndex() == k->initFrame) {
        if (k->editMode == TupToolPlugin::Path) {
            if (k->nodesGroup) {
                k->nodesGroup->createNodes(k->path);
                k->nodesGroup->expandAllNodes();
                k->configurator->updateSteps(k->path);
                QPainterPath::Element e = k->path->path().elementAt(0);
                QPointF begin = QPointF(e.x, e.y);

                if (begin != k->firstNode) {
                    QPointF oldPos = k->firstNode;
                    QPointF newPos = begin;

                    int distanceX = newPos.x() - oldPos.x();
                    int distanceY = newPos.y() - oldPos.y();

                    if (k->objects.size() > 0) {
                        foreach (QGraphicsItem *item, k->objects)
                                 item->moveBy(distanceX, distanceY);
                        QGraphicsItem *item = k->objects.at(0);
                        QRectF rect = item->sceneBoundingRect();
                        k->itemObjectReference = rect.center();
                    }

                    k->firstNode = newPos;
                }
            }
        } else {
            if (scene->selectedItems().size() > 0) {
                k->objects = scene->selectedItems();
                k->configurator->notifySelection(true);

                QGraphicsItem *item = k->objects.at(0);
                QRectF rect = item->sceneBoundingRect();
                QPointF newPos = rect.center();
                QPointF oldPos = k->itemObjectReference;
                k->itemObjectReference = newPos;

                if (!k->path) {
                    k->path = new QGraphicsPathItem;
                    k->path->setZValue(k->baseZValue);

                    QColor color(55, 155, 55, 200);
                    QPen pen(QBrush(color), 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
                    k->path->setPen(pen);

                    QPainterPath path;
                    path.moveTo(newPos);
                    k->firstNode = newPos;
                    k->path->setPath(path);
                    scene->addItem(k->path);
                    k->isPathInScene = true;

                    k->pathOffset = QPointF(0, 0);
                } else {
                    int distanceX = newPos.x() - oldPos.x();
                    int distanceY = newPos.y() - oldPos.y();
                    k->path->moveBy(distanceX, distanceY);

                    k->pathOffset = QPointF(distanceX, distanceY);
                }
            } 
        }
    } 
}

/* This method returns the list of actions defined in this plugin */

QMap<QString, TAction *> Tweener::actions() const
{
    return k->actions;
}

/* This method returns the list of actions defined in this plugin */

int Tweener::toolType() const
{
    return TupToolInterface::Tweener;
}

/* This method returns the tool panel associated to this plugin */

QWidget *Tweener::configurator()
{
    if (!k->configurator) {
        k->mode = TupToolPlugin::View;

        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(startingFrameChanged(int)), this, SLOT(updateStartFrame(int)));
        connect(k->configurator, SIGNAL(clickedCreatePath()), this, SLOT(setTweenPath()));
        connect(k->configurator, SIGNAL(clickedSelect()), this, SLOT(setSelection()));
        connect(k->configurator, SIGNAL(clickedRemoveTween(const QString &)), this, SLOT(removeTween(const QString &)));
        connect(k->configurator, SIGNAL(clickedResetInterface()), this, SLOT(applyReset()));
        connect(k->configurator, SIGNAL(setMode(TupToolPlugin::Mode)), this, SLOT(updateMode(TupToolPlugin::Mode)));
        connect(k->configurator, SIGNAL(clickedApplyTween()), this, SLOT(applyTween()));
        connect(k->configurator, SIGNAL(getTweenData(const QString &)), this, SLOT(setCurrentTween(const QString &)));
    } else {
        k->mode = k->configurator->mode();
    }

    return k->configurator;
}

/* This method is called when there's a change on/of scene */
void Tweener::aboutToChangeScene(TupGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

/* This method is called when this plugin is off */

void Tweener::aboutToChangeTool()
{
    resetGUI();
}

void Tweener::resetGUI()
{
    if (k->editMode == TupToolPlugin::Selection) {
        clearSelection();
        disableSelection();
        return;
    }

    if (k->editMode == TupToolPlugin::Path) {
        if (k->path) {
            k->scene->removeItem(k->path);
            k->isPathInScene = false;
            if (k->nodesGroup) {
                k->nodesGroup->clear();
                k->nodesGroup = 0;
            }
        }
        return;
    }
}

/* This method defines the actions contained in this plugin */

void Tweener::setupActions()
{
    TAction *action = new TAction(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "position_tween.png"), tr("Position Tween"), this);
    action->setCursor(QCursor(kAppProp->themeDir() + "cursors" + QDir::separator() + "tweener.png"));
    action->setShortcut(QKeySequence(tr("Shift+W")));

    k->actions.insert(tr("Position Tween"), action);
}

/* This method initializes the "Create path" mode */

void Tweener::setTweenPath()
{
    if (k->path) {
        k->pathOffset = QPointF(0, 0);

        if (!k->isPathInScene) {
            k->scene->addItem(k->path);
            k->isPathInScene = true;
        } 

        if (k->nodesGroup) {
            k->nodesGroup->createNodes(k->path);
        } else {
            k->nodesGroup = new TNodeGroup(k->path, k->scene, TNodeGroup::PositionTween, k->baseZValue);
            connect(k->nodesGroup, SIGNAL(nodeReleased()), SLOT(updatePath()));
            k->nodesGroup->createNodes(k->path);
        }
        k->nodesGroup->expandAllNodes();
    }

    k->editMode = TupToolPlugin::Path;
    disableSelection();
}

/* This method initializes the "Select object" mode */

void Tweener::setSelection()
{
    if (k->mode == TupToolPlugin::Edit) {
        if (k->initFrame != k->scene->currentFrameIndex()) {
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentTween->initScene(),
                                                                              k->currentTween->initLayer(),
                                                                              k->currentTween->initFrame(), 
                                                                              TupProjectRequest::Select, "1");
            emit requested(&request);
        }
    }

    if (k->path) {
        k->scene->removeItem(k->path);
        k->isPathInScene = false;
        if (k->nodesGroup) {
            k->nodesGroup->clear();
            k->nodesGroup = 0;
        }
    }

    k->editMode = TupToolPlugin::Selection;

    int bottomBoundary = 20000 + (k->scene->currentLayerIndex()*10000);
    int topBoundary = bottomBoundary + 10000; 

    foreach (QGraphicsView * view, k->scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      if ((item->zValue() >= bottomBoundary) && (item->zValue() < topBoundary) && (item->toolTip().length()==0)) 
                           item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
             }
    }

    // When Object selection is enabled, previous selection is set
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                 item->setSelected(true);
        }
    }
}

/* This method transforms the path created into a QString representation */

QString Tweener::pathToCoords()
{
    QString strPath = "";
    QChar t;
    int offsetX = k->pathOffset.x();
    int offsetY = k->pathOffset.y();

    for (int i=0; i < k->path->path().elementCount(); i++) {
         QPainterPath::Element e = k->path->path().elementAt(i);
         switch (e.type) {
             case QPainterPath::MoveToElement:
             {
                 if (t != 'M') {
                     t = 'M';
                     strPath += "M " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                 } else {
                     strPath += QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                 }
             }
             break;
             case QPainterPath::LineToElement:
             {
                  if (t != 'L') {
                      t = 'L';
                      strPath += " L " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  } else {
                      strPath += QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  }
             }
             break;
             case QPainterPath::CurveToElement:
             {
                  if (t != 'C') {
                      t = 'C';
                      strPath += " C " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  } else {
                      strPath += "  " + QString::number(e.x + offsetX) + " " + QString::number(e.y + offsetY) + " ";
                  }
             }
             break;
             case QPainterPath::CurveToDataElement:
             {
                  if (t == 'C')
                      strPath +=  " " + QString::number(e.x + offsetX) + "  " + QString::number(e.y + offsetY) + " ";
             }
             break;
         }
    }

    return strPath.trimmed();
}

/* This method resets this plugin */

void Tweener::applyReset()
{
    k->mode = TupToolPlugin::View;
    k->editMode = TupToolPlugin::None;

    clearSelection();
    disableSelection();

    if (k->nodesGroup) {
        k->nodesGroup->clear();
        k->nodesGroup = 0;
    }

    if (k->path) {
        if (k->initFrame == k->scene->currentFrameIndex())
            k->scene->removeItem(k->path);
        k->isPathInScene = false;
        k->path = 0;
    }

    k->initFrame = k->scene->currentFrameIndex();
    k->initLayer = k->scene->currentLayerIndex();
    k->initScene = k->scene->currentSceneIndex();

    k->configurator->cleanData();
}

/* This method applies to the project, the Tween created from this plugin */

void Tweener::applyTween()
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    QString name = k->configurator->currentTweenName();

    if (name.length() == 0) {
        TOsd::self()->display(tr("Error"), tr("Tween name is missing!"), TOsd::Error);
        return;
    }

    if (!k->scene->scene()->tweenExists(name, TupItemTweener::Position)) {
        k->initFrame = k->scene->currentFrameIndex();
        k->initLayer = k->scene->currentLayerIndex();
        k->initScene = k->scene->currentSceneIndex();

        foreach (QGraphicsItem *item, k->objects) {   
                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 int objectIndex = k->scene->currentFrame()->indexOf(item); 
                 QRectF rect = item->sceneBoundingRect();
                 QPointF point = rect.topLeft();

                 if (TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item)) {
                     type = TupLibraryObject::Svg;
                     objectIndex = k->scene->currentFrame()->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<TupPathItem *>(item) || qgraphicsitem_cast<TupEllipseItem *>(item) 
                         || qgraphicsitem_cast<TupLineItem *>(item) || qgraphicsitem_cast<TupRectItem *>(item))
                         point = item->pos();
                 }

                 QString route = pathToCoords();

                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                             k->initScene, k->initLayer, k->initFrame,
                                             objectIndex,
                                             QPointF(), k->scene->spaceMode(), type,
                                             TupProjectRequest::SetTween, 
                                             k->configurator->tweenToXml(k->initScene, k->initLayer, k->initFrame, point, route));
                 emit requested(&request);
        }
    } else {
        removeTweenFromProject(name);
        QList<QGraphicsItem *> newList;

        k->initFrame = k->configurator->startFrame();
        k->initLayer = k->currentTween->initLayer();
        k->initScene = k->currentTween->initScene();

        foreach (QGraphicsItem *item, k->objects) {
                 TupLibraryObject::Type type = TupLibraryObject::Item;
                 TupProject *project = k->scene->scene()->project();
                 TupScene *scene = project->scene(k->initScene);
                 TupLayer *layer = scene->layer(k->initLayer);
                 TupFrame *frame = layer->frame(k->currentTween->initFrame());
                 int objectIndex = frame->indexOf(item);
                 QRectF rect = item->sceneBoundingRect();
                 QPointF point = rect.topLeft();
                 TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item); 

                 if (svg) {
                     type = TupLibraryObject::Svg;
                     objectIndex = frame->indexOf(svg);
                 } else {
                     if (qgraphicsitem_cast<TupPathItem *>(item) || qgraphicsitem_cast<TupEllipseItem *>(item) 
                         || qgraphicsitem_cast<TupLineItem *>(item) || qgraphicsitem_cast<TupRectItem *>(item))
                         point = item->pos();
                 }

                 if (k->initFrame != k->currentTween->initFrame()) {
                     QDomDocument dom;
                     if (type == TupLibraryObject::Svg)
                         dom.appendChild(svg->toXml(dom));
                     else
                         dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));

                     TupProjectRequest request = TupRequestBuilder::createItemRequest(k->initScene, k->initLayer, 
                                                 k->initFrame, 0, QPointF(), 
                                                 k->scene->spaceMode(), type, 
                                                 TupProjectRequest::Add, dom.toString());
                     emit requested(&request);

                     request = TupRequestBuilder::createItemRequest(k->currentTween->initScene(), 
                               k->currentTween->initLayer(),
                               k->currentTween->initFrame(),
                               objectIndex, QPointF(), 
                               k->scene->spaceMode(), type,
                               TupProjectRequest::Remove);
                     emit requested(&request);

                     frame = layer->frame(k->initFrame);
                     if (type == TupLibraryObject::Item) {
                         objectIndex = frame->graphicItemsCount() - 1;
                         newList.append(frame->graphic(objectIndex)->item());
                     } else {
                         objectIndex = frame->svgItemsCount() - 1;
                         newList.append(frame->svg(objectIndex));
                     }
                 }

                 QString route = pathToCoords();

                 TupProjectRequest request = TupRequestBuilder::createItemRequest(
                                             k->initScene, k->initLayer, k->initFrame,
                                             objectIndex,
                                             QPointF(), k->scene->spaceMode(), type,
                                             TupProjectRequest::SetTween,
                                             k->configurator->tweenToXml(k->initScene, k->initLayer, k->initFrame, point, route));
                 emit requested(&request);
        }

        if (newList.size() > 0)
            k->objects = newList;
    }

    int framesNumber = framesTotal();
    int total = k->initFrame + k->configurator->totalSteps() - 1;
    int layersTotal = k->scene->scene()->layersTotal();
    TupProjectRequest request;

    if (total > framesNumber) {
        for (int i = framesNumber; i <= total; i++) {
             for (int j = 0; j < layersTotal; j++) {
                  request = TupRequestBuilder::createFrameRequest(k->initScene, j, i, TupProjectRequest::Add,
                                                                  tr("Frame %1").arg(i + 1));
                  emit requested(&request);
             }
        }
    }

    request = TupRequestBuilder::createFrameRequest(k->initScene, k->initLayer, k->initFrame,
                                                    TupProjectRequest::Select, "1");
    emit requested(&request);

    setCurrentTween(name);
    TOsd::self()->display(tr("Info"), tr("Tween %1 applied!").arg(name), TOsd::Info);
}

/* This method updates the data of the path into the tool panel 
 * and disables edition mode for nodes
*/

void Tweener::updatePath()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->configurator->updateSteps(k->path);
}

/* This method saves the settings of this plugin */

void Tweener::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void Tweener::updateScene(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->mode = k->configurator->mode();

    if (k->mode == TupToolPlugin::Edit) {
        int total = k->initFrame + k->configurator->totalSteps();

        if (k->editMode == TupToolPlugin::Path) {
            if (scene->currentFrameIndex() >= k->initFrame && scene->currentFrameIndex() < total) {
                if (k->path && k->nodesGroup) {
                    k->scene->addItem(k->path);            
                    k->nodesGroup->createNodes(k->path);
                    k->nodesGroup->expandAllNodes();
                }
            }
        } 

        int framesNumber = framesTotal();

        if (k->configurator->startComboSize() < framesNumber)
            k->configurator->initStartCombo(framesNumber, k->initFrame);

    } else if (k->mode == TupToolPlugin::Add) {
               int total = framesTotal();
               if (k->configurator->startComboSize() < total) {
                   k->configurator->initStartCombo(total, k->initFrame);
               } else {
                   if (scene->currentFrameIndex() != k->initFrame)
                       k->configurator->setStartFrame(scene->currentFrameIndex());
               }

           if (k->editMode == TupToolPlugin::Path) {
               k->path = 0;
               k->configurator->cleanData();
               k->configurator->activateMode(TupToolPlugin::Selection);
               clearSelection();
               setSelection();
           } else if (k->editMode == TupToolPlugin::Selection) {
                      k->path = 0;
                      if (scene->currentFrameIndex() != k->initFrame)
                          clearSelection();
                      k->initFrame = scene->currentFrameIndex();
                      setSelection();
           } 
    } else {
        if (scene->currentFrameIndex() != k->initFrame) 
            k->configurator->setStartFrame(scene->currentFrameIndex());
    }
}

void Tweener::updateMode(TupToolPlugin::Mode mode)
{
    k->mode = mode;

    if (k->mode == TupToolPlugin::Edit)
        setEditEnv();
}

void Tweener::removeTweenFromProject(const QString &name)
{
    TupScene *scene = k->scene->scene();
    scene->removeTween(name, TupItemTweener::Position);

    foreach (QGraphicsView * view, k->scene->views()) {
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      QString tip = item->toolTip();
                      if (tip.startsWith(tr("Position Tween") + ": " + name))
                          item->setToolTip("");
             }
    }
}

void Tweener::removeTween(const QString &name)
{
    removeTweenFromProject(name);
    applyReset();
}

void Tweener::setCurrentTween(const QString &name)
{
    TupScene *scene = k->scene->scene();
    k->currentTween = scene->tween(name, TupItemTweener::Position);
    if (k->currentTween)
        k->configurator->setCurrentTween(k->currentTween);
}

void Tweener::setEditEnv()
{
    k->initFrame = k->currentTween->initFrame();
    k->initLayer = k->currentTween->initLayer();
    k->initScene = k->currentTween->initScene();

    if (k->initFrame != k->scene->currentFrameIndex() || k->initLayer != k->scene->currentLayerIndex() || k->initScene != k->scene->currentSceneIndex()) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->initScene, k->initLayer, k->initFrame, TupProjectRequest::Select, "1");
        emit requested(&request);
    }

    k->mode = TupToolPlugin::Edit;

    TupScene *scene = k->scene->scene();
    k->objects = scene->getItemsFromTween(k->currentTween->name(), TupItemTweener::Position);
    QGraphicsItem *item = k->objects.at(0);
    QRectF rect = item->sceneBoundingRect();
    k->itemObjectReference = rect.center();

    k->path = k->currentTween->graphicsPath();
    k->path->setZValue(k->baseZValue);

    QPainterPath::Element e = k->path->path().elementAt(0);
    k->firstNode = QPointF(e.x, e.y);

    QPointF oldPos = QPointF(e.x, e.y);
    QPointF newPos = rect.center();

    int distanceX = newPos.x() - oldPos.x();
    int distanceY = newPos.y() - oldPos.y();
    k->path->moveBy(distanceX, distanceY);
    k->pathOffset = QPointF(distanceX, distanceY);

    QColor color(55, 155, 55, 200);
    QPen pen(QBrush(color), 2, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    k->path->setPen(pen);

    setTweenPath();
}

int Tweener::framesTotal()
{
    int total = 1;
    TupLayer *layer = k->scene->scene()->layer(k->scene->currentLayerIndex());
    if (layer)
        total = layer->framesTotal();

    return total;
}

/* This method clear selection */

void Tweener::clearSelection()
{
    if (k->objects.size() > 0) {
        foreach (QGraphicsItem *item, k->objects) {
                 if (item->isSelected())
                     item->setSelected(false);
        }
        k->objects.clear();
        k->configurator->notifySelection(false);
    }
}

/* This method disables object selection */

void Tweener::disableSelection()
{
    foreach (QGraphicsView *view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

void Tweener::sceneResponse(const TupSceneResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if ((event->action() == TupProjectRequest::Remove || event->action() == TupProjectRequest::Reset)
        && (k->scene->currentSceneIndex() == event->sceneIndex())) {
        init(k->scene);
    }

    if (event->action() == TupProjectRequest::Select)
        init(k->scene);
}

void Tweener::layerResponse(const TupLayerResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (event->action() == TupProjectRequest::Remove)
        init(k->scene);        
}

void Tweener::frameResponse(const TupFrameResponse *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (event->action() == TupProjectRequest::Remove && k->scene->currentLayerIndex() == event->layerIndex())
        init(k->scene);

    if (event->action() == TupProjectRequest::Select) {
        if (k->initLayer != event->layerIndex() || k->initScene != event->sceneIndex()) {
            resetGUI();
            init(k->scene);
        }
    }
}

TupToolPlugin::Mode Tweener::currentMode()
{
    return k->mode;
}

TupToolPlugin::EditMode Tweener::currentEditMode()
{
    return k->editMode;
}

// Q_EXPORT_PLUGIN2(tup_tweener, Tweener);
