/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                          *
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

#include "tuppaintarea.h"
#include "tupbrushmanager.h"
#include "tupinputdeviceinformation.h"

#include "tuppaintareaevent.h"
#include "tuppaintarearotator.h"
#include "tupimagedevice.h"
#include "tupgraphicsscene.h"
#include "tconfig.h"
#include "tapplication.h"
#include "tdebug.h"
#include "tuptextitem.h"
#include "tuplibrarydialog.h"
#include "tuplibraryobject.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tupprojectresponse.h"

#include "tupscene.h"
#include "tuplayer.h"
#include "tupsvgitem.h"
#include "tuppixmapitem.h"
#include "node.h"
#include "tcontrolnode.h"
#include "tupproject.h"
#include "tosd.h"

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QPolygon>
#include <QApplication>
// #include <QTimer>
#include <QStyleOptionGraphicsItem>
#include <QClipboard>
#include <QMenu>

/**
 * This class defines the behavior of the main paint area when ilustration module is on
 * Here is where all the events about the paint area are processed.
 * @author David Cuadrado
*/

struct TupPaintArea::Private
{
    TupProject *project;
    int currentSceneIndex;
    QStringList copiesXml;
    QString currentTool; 
    bool deleteMode;
    TupProject::Mode spaceMode;
    QPointF oldPosition;
    QPointF position;
    bool menuOn;
    QString frameCopy;
    bool canvasEnabled;
};

TupPaintArea::TupPaintArea(TupProject *project, QWidget * parent) : TupPaintAreaBase(parent, project->dimension()), k(new Private)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    k->canvasEnabled = false;

    k->project = project;
    setBgColor(project->bgColor());
    k->currentSceneIndex = 0;
    k->deleteMode = false;
    k->menuOn = false;

    setCurrentScene(0);
    k->currentTool = tr("Pencil");

    if (graphicsScene()->scene())
        graphicsScene()->setCurrentFrame(0, 0);
}

TupPaintArea::~TupPaintArea()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    delete k;
}

void TupPaintArea::setCurrentScene(int index)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->project->scenesTotal() > 0) {
        TupScene *scene = k->project->scene(index);
        if (scene) {
            k->currentSceneIndex = index;
            graphicsScene()->setCurrentScene(scene);
        } else {
            if (k->project->scenesTotal() == 1) {
                setDragMode(QGraphicsView::NoDrag);
                k->currentSceneIndex = 0;
                graphicsScene()->setCurrentScene(0);
            } else {
                #ifdef K_DEBUG
                       tError() << "TupPaintArea::setCurrentScene() - [ Fatal Error ] -  No scenes available. Invalid index -> " << index;
                       tError() << "TupPaintArea::setCurrentScene() - Scenes total -> " << k->project->scenesTotal();
                #endif
            }
        }
    }
}

void TupPaintArea::mousePressEvent(QMouseEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!k->canvasEnabled)
        return;

    if (graphicsScene()->currentFrame()->isLocked()) {
        #ifdef K_DEBUG
               tFatal() << "TupPaintArea::mousePressEvent() - Frame is locked!";
        #endif
        return;
    }

    if (k->currentTool.compare(tr("Line Selection")) == 0) {
        // If a node is the target... abort!
        if (event->buttons() == Qt::RightButton) {
            // if (qgraphicsitem_cast<TControlNode *>(scene()->itemAt(mapToScene(event->pos()))))
            if (qgraphicsitem_cast<TControlNode *>(scene()->itemAt(mapToScene(event->pos()), QTransform())))
                return;
        }
    }

    if (k->currentTool.compare(tr("PolyLine")) == 0) {
        if (event->buttons() == Qt::RightButton) {
            emit closePolyLine();
            return;
        }
    }

    if (k->currentTool.compare(tr("Object Selection")) == 0) {
        if (event->buttons() == Qt::RightButton) {
            // If a node is the target... abort!
            // if (qgraphicsitem_cast<Node *>(scene()->itemAt(mapToScene(event->pos()))))
            if (qgraphicsitem_cast<Node *>(scene()->itemAt(mapToScene(event->pos()), QTransform())))
                return;

            // if (QGraphicsItem *item = scene()->itemAt(mapToScene(event->pos()))) {
            if (QGraphicsItem *item = scene()->itemAt(mapToScene(event->pos()), QTransform())) {
                if (item->opacity() == 1) {
                    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
                    // item->setSelected(true);
                } else {
                    return;
                }
            }

            QMenu *menu = new QMenu(tr("Drawing area"));
            menu->addAction(kApp->findGlobalAction("undo"));
            menu->addAction(kApp->findGlobalAction("redo"));
            menu->addSeparator();

            QAction *cut = menu->addAction(tr("Cut"), this, SLOT(cutItems()), QKeySequence(tr("Ctrl+X")));
            QAction *copy = menu->addAction(tr("Copy"), this, SLOT(copyItems()), QKeySequence(tr("Ctrl+C")));
            QAction *paste = menu->addAction(tr("Paste"), this, SLOT(pasteItems()), QKeySequence(tr("Ctrl+V")));

            QMenu *pasteMenu = new QMenu(tr("Paste in..."));
            QAction *pasteFive = pasteMenu->addAction(tr("next 5 frames"), this, SLOT(pasteNextFive()));
            QAction *pasteTen = pasteMenu->addAction(tr("next 10 frames"), this, SLOT(pasteNextTen()));
            QAction *pasteTwenty = pasteMenu->addAction(tr("next 20 frames"), this, SLOT(pasteNextTwenty()));
            QAction *pasteFifty = pasteMenu->addAction(tr("next 50 frames"), this, SLOT(pasteNextFifty()));
            QAction *pasteHundred = pasteMenu->addAction(tr("next 100 frames"), this, SLOT(pasteNextHundred()));

            pasteMenu->addAction(pasteFive);
            pasteMenu->addAction(pasteTen);
            pasteMenu->addAction(pasteTwenty);
            pasteMenu->addAction(pasteFifty);
            pasteMenu->addAction(pasteHundred);

            menu->addMenu(pasteMenu);

            QAction *del = menu->addAction(tr("Delete"), this, SLOT(deleteItems()), QKeySequence(Qt::Key_Delete));

            menu->addSeparator();
            QMenu *order = new QMenu(tr("Send"));

            connect(order, SIGNAL(triggered(QAction*)), this, SLOT(requestItemMovement(QAction*)));
            order->addAction(tr("To back"))->setData(TupFrame::MoveBack);
            order->addAction(tr("To front"))->setData(TupFrame::MoveToFront);
            order->addAction(tr("One level to back"))->setData(TupFrame::MoveOneLevelBack);
            order->addAction(tr("One level to front"))->setData(TupFrame::MoveOneLevelToFront);

            menu->addMenu(order);
            menu->addSeparator();

            // Code commented temporary while SQA is done
            QAction *addItem = menu->addAction(tr("Add to library..."), this, SLOT(addSelectedItemsToLibrary()));
            menu->addSeparator();

            if (scene()->selectedItems().isEmpty()) {
                del->setEnabled(false);
                cut->setEnabled(false);
                copy->setEnabled(false);
                addItem->setEnabled(false);
            } else {
                QList<QGraphicsItem *> selected = scene()->selectedItems();
                foreach (QGraphicsItem *item, selected) {
                         QDomDocument dom;
                         dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));
		         QDomElement root = dom.documentElement();

                         if (root.tagName() == "symbol") {
                             QString key = root.attribute("id").toUpper();
                             if (key.endsWith("JPG") || key.endsWith("PNG") || key.endsWith("GIF") || key.endsWith("XPM")) {
                                 addItem->setEnabled(false);
                                 break;
                             }
                         } else if (root.tagName() == "svg") {
                                    addItem->setEnabled(false);
                                    break;
                         }
                }
            }

            if (k->copiesXml.isEmpty()) {
                paste->setEnabled(false);
                pasteMenu->setEnabled(false);    
            }

            if (QMenu *toolMenu = graphicsScene()->currentTool()->menu()) {
                menu->addSeparator();
                menu->addMenu(toolMenu);
            }

            k->position = viewPosition();
            k->menuOn = true;
            menu->exec(event->globalPos());
        }
    } 

    TupPaintAreaBase::mousePressEvent(event);
}

void TupPaintArea::frameResponse(TupFrameResponse *event)
{
    #ifdef K_DEBUG
           tDebug() << "TupPaintArea::frameResponse() - [" << event->sceneIndex() << ", " << event->layerIndex() << ", " << event->frameIndex() << "]";
    #endif

    TupGraphicsScene *guiScene = graphicsScene();

    if (!guiScene->scene())
        return;

    if (!guiScene->isDrawing()) {
        switch (event->action()) {
                case TupProjectRequest::Add:
                    {
                    }
                break; 
                case TupProjectRequest::Select:
                case TupProjectRequest::Paste:
                case TupProjectRequest::Reset:
                    {
                        if (event->action() == TupProjectRequest::Select) {
                            if (guiScene->currentFrameIndex() != event->frameIndex())
                                emit frameChanged(event->frameIndex());
                        }

                        // SQA: Check if this instruction is really required
                        // setUpdatesEnabled(true);

                        guiScene->setCurrentFrame(event->layerIndex(), event->frameIndex());

                        if (k->spaceMode == TupProject::FRAMES_EDITION) {
                            guiScene->drawPhotogram(event->frameIndex(), true);
                        } else {
                            guiScene->cleanWorkSpace();
                            guiScene->drawBackground(guiScene->currentFrameIndex());
                        }

                        if (guiScene->currentTool()->toolType() == TupToolInterface::Selection)
                            guiScene->currentTool()->init(graphicsScene());
                    }
                break;
                case TupProjectRequest::Lock:
                    {
                        if (guiScene->currentFrameIndex() == event->frameIndex())
                            viewport()->update();
                    }
                    break;
                case TupProjectRequest::Remove:
                    {
                        if (event->frameIndex() == 0) {
                            guiScene->cleanWorkSpace();
                            viewport()->update();
                        }
                    }
                    break;
                default:
                    break;
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "TupPaintArea::frameResponse() - isDrawing() == true! - No action taken!";
        #endif
    }

    guiScene->frameResponse(event);
}

void TupPaintArea::layerResponse(TupLayerResponse *event)
{
    #ifdef K_DEBUG
           tDebug() << "TupPaintArea::layerResponse() - [" << event->sceneIndex() << ", " << event->layerIndex() << "]";
    #endif

    if (event->action() == TupProjectRequest::Add)
        return;

    TupGraphicsScene *guiScene = graphicsScene();

    if (!guiScene->scene())
        return;

    int frameIndex = guiScene->currentFrameIndex();

    if (event->action() == TupProjectRequest::View)
        guiScene->setLayerVisible(event->layerIndex(), event->arg().toBool());

    if (event->action() != TupProjectRequest::Add && event->action() != TupProjectRequest::Remove) {
        if (k->spaceMode == TupProject::FRAMES_EDITION) {
            guiScene->drawCurrentPhotogram();
        } else {
            guiScene->cleanWorkSpace();
            guiScene->drawBackground(frameIndex);
        }

        viewport()->update(scene()->sceneRect().toRect());
    } else {
        if (event->action() == TupProjectRequest::Remove) {
            TupScene *scene = k->project->scene(k->currentSceneIndex);

            if (scene->layersTotal() > 1) {
                if (event->layerIndex() != 0)
                    guiScene->setCurrentFrame(event->layerIndex() - 1, frameIndex);
                else 
                    guiScene->setCurrentFrame(event->layerIndex() + 1, frameIndex);

                if (k->spaceMode == TupProject::FRAMES_EDITION) {
                    guiScene->drawCurrentPhotogram();
                } else {
                    guiScene->cleanWorkSpace();
                    guiScene->drawBackground(frameIndex);
                }
            } else {
                if (scene->layersTotal() == 1) {
                    QList<int> indexes = scene->layerIndexes();

                    guiScene->setCurrentFrame(indexes.at(0), frameIndex);                
                    if (k->spaceMode == TupProject::FRAMES_EDITION) {
                        guiScene->drawCurrentPhotogram();
                    } else {
                        guiScene->cleanWorkSpace();
                        guiScene->drawBackground(frameIndex);
                    }
                } 
            }

            viewport()->update();
        }
    }

    guiScene->layerResponse(event);
}

void TupPaintArea::sceneResponse(TupSceneResponse *event)
{
    #ifdef K_DEBUG
           tDebug() << "TupPaintArea::sceneResponse() - [" << event->sceneIndex() << "]";
    #endif

    TupGraphicsScene *guiScene = graphicsScene();

    if (!guiScene->scene())
        return;

    if (!guiScene->isDrawing()) {
        switch(event->action()) {
            case TupProjectRequest::Select:
                {
                    if (event->sceneIndex() >= 0) {
                        if (k->project->scenesTotal() == 1)
                            setCurrentScene(0);
                        else
                            setCurrentScene(event->sceneIndex());
                    }
                }
                break;
            case TupProjectRequest::Remove:
                {
                    /*
                    if (k->project->scenesTotal() > 0)
                        setCurrentScene(k->currentSceneIndex);
                    */
                }
                break;
            case TupProjectRequest::Reset:
                {
                    setCurrentScene(0);
                }
                break;
            case TupProjectRequest::BgColor:
                {
                    QString colorName = event->arg().toString();
                    QColor color(colorName);
                    setBgColor(color);
                    emit updateStatusBgColor(color);
                }
                break;
            default: 
                {
                    #ifdef K_DEBUG
                           tFatal() << "TupPaintArea::sceneResponse <- TupProjectRequest::Default";
                    #endif
                }
                break;
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "TupPaintArea::sceneResponse() - isDrawing() == true! - No action taken!";
        #endif
    }

    guiScene->sceneResponse(event);
}

void TupPaintArea::itemResponse(TupItemResponse *event)
{
    #ifdef K_DEBUG
           tDebug() << "TupPaintArea::itemResponse() - [" << event->sceneIndex() << ", " << event->layerIndex() << ", " << event->frameIndex() << "]";
    #endif

    TupGraphicsScene *guiScene = graphicsScene();

    if (!guiScene->scene())
        return;

    if (!guiScene->isDrawing()) {
        switch(event->action()) {
            case TupProjectRequest::Transform:
                 {
                     viewport()->update();
                 }
                 break;

           case TupProjectRequest::Remove:
                { 
                     if (!k->deleteMode) {
                         if (k->spaceMode == TupProject::FRAMES_EDITION) {
                             guiScene->drawCurrentPhotogram();
                         } else {
                             guiScene->cleanWorkSpace();
                             guiScene->drawBackground(guiScene->currentFrameIndex());
                         }

                         viewport()->update(scene()->sceneRect().toRect());
                     } 
                }
                break;

           default:
                {
                     if (k->spaceMode == TupProject::FRAMES_EDITION) {
                        guiScene->drawCurrentPhotogram();
                     } else {
                        guiScene->cleanWorkSpace();
                        guiScene->drawBackground(guiScene->currentFrameIndex());
                     }

                     viewport()->update(scene()->sceneRect().toRect());

                     if (guiScene->currentTool()->toolType() != TupToolInterface::Tweener && k->currentTool.compare(tr("PolyLine")) != 0)
                         guiScene->currentTool()->init(graphicsScene());          
                }
                break;
            }
    } else {
        #ifdef K_DEBUG
               tFatal() << "TupPaintArea::itemResponse() - isDrawing() == true! - No action taken!";
        #endif
    }

    guiScene->itemResponse(event);
}

void TupPaintArea::projectResponse(TupProjectResponse *)
{
}

void TupPaintArea::libraryResponse(TupLibraryResponse *request)
{
    #ifdef K_DEBUG
           tDebug() << "TupPaintArea::libraryResponse() - Request Action: " << request->action();
    #endif

    TupGraphicsScene *guiScene = graphicsScene();

    if (!guiScene->scene())
        return;

    if (!guiScene->isDrawing()) {
        int frameIndex = guiScene->currentFrameIndex();
        switch (request->action()) {
                case TupProjectRequest::InsertSymbolIntoFrame:
                    {
                         if (k->spaceMode == TupProject::FRAMES_EDITION) {
                             guiScene->drawCurrentPhotogram();
                         } else {
                             guiScene->cleanWorkSpace();
                             guiScene->drawBackground(frameIndex);
                         }

                         viewport()->update(scene()->sceneRect().toRect());

                         if (k->currentTool.compare(tr("Object Selection")) == 0)
                             emit itemAddedOnSelection(guiScene);
                     }
                     break;
                     case TupProjectRequest::Remove:
                     case TupProjectRequest::RemoveSymbolFromFrame:
                     {
                         if (k->spaceMode == TupProject::FRAMES_EDITION) {
                             guiScene->drawCurrentPhotogram();
                         } else {
                             guiScene->cleanWorkSpace();
                             guiScene->drawBackground(frameIndex);
                         }

                         viewport()->update(scene()->sceneRect().toRect());
                     }
                     break;
        } 
    } else {
        #ifdef K_DEBUG
               tFatal() << "TupPaintArea::libraryResponse() - isDrawing() == true! - No action taken!";
        #endif
    }
}

bool TupPaintArea::canPaint() const
{
    TupGraphicsScene *guiScene = graphicsScene();

    if (guiScene->scene()) {
        if (guiScene->currentFrameIndex() >= 0 && guiScene->currentLayerIndex() >= 0) 
            return true;
    }

    return false;
}

void TupPaintArea::deleteItems()
{
    #ifdef K_DEBUG
           T_FUNCINFOX("paintarea");
    #endif

    if (k->currentTool.compare(tr("Object Selection")) != 0 && k->currentTool.compare(tr("Line Selection")) != 0)
        return;

    QList<QGraphicsItem *> selected = scene()->selectedItems();

    if (!selected.empty()) {
        TupGraphicsScene* currentScene = graphicsScene();

        if (currentScene) {
            int counter = 0;
            int total = selected.count();
            k->deleteMode = true;
            foreach (QGraphicsItem *item, selected) {
                     if (counter == total-1) 
                         k->deleteMode = false;

                     TupLibraryObject::Type type;

                     TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
                     int itemIndex = -1;

                     if (svg) {
                         type = TupLibraryObject::Svg;
                         if (k->spaceMode == TupProject::FRAMES_EDITION) {
                             itemIndex = currentScene->currentFrame()->indexOf(svg);
                         } else if (k->spaceMode == TupProject::STATIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = currentScene->scene()->background();
                                    if (bg) {
                                        TupFrame *frame = bg->staticFrame();
                                        if (frame) {
                                            itemIndex = frame->indexOf(svg);
                                        } else {
                                            #ifdef K_DEBUG
                                                   tError() << "TupPaintArea::deleteItems() - Fatal Error: Background frame is NULL!";
                                            #endif
                                        }
                                    } else {
                                        #ifdef K_DEBUG
                                               tError() << "TupPaintArea::deleteItems() - Fatal Error: Scene has no background element!";
                                        #endif
                                    }
                         } else if (k->spaceMode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = currentScene->scene()->background();
                                    if (bg) {
                                        TupFrame *frame = bg->dynamicFrame();
                                        if (frame) {
                                            itemIndex = frame->indexOf(svg);
                                        } else {
                                            #ifdef K_DEBUG
                                                   tError() << "TupPaintArea::deleteItems() - Fatal Error: Background frame is NULL!";
                                            #endif
                                        }
                                    } else {
                                        #ifdef K_DEBUG
                                               tError() << "TupPaintArea::deleteItems() - Fatal Error: Scene has no background element!";
                                        #endif
                                    }
                         } else {
                             #ifdef K_DEBUG
                                    tError() << "TupPaintArea::deleteItems() - Fatal Error: invalid spaceMode!";
                             #endif
                         }
                     } else {
                         type = TupLibraryObject::Item;
                         if (k->spaceMode == TupProject::FRAMES_EDITION) {
                             itemIndex = currentScene->currentFrame()->indexOf(item);
                         } else if (k->spaceMode == TupProject::STATIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = currentScene->scene()->background();
                                    if (bg) {
                                        TupFrame *frame = bg->staticFrame();
                                        if (frame) {
                                            itemIndex = frame->indexOf(item);
                                        } else {
                                            #ifdef K_DEBUG
                                                   tError() << "TupPaintArea::deleteItems() - Fatal Error: Background frame is NULL!";
                                            #endif
                                        }
                                    } else {
                                        #ifdef K_DEBUG
                                               tError() << "TupPaintArea::deleteItems() - Fatal Error: Scene has no background element!";
                                        #endif
                                    }
                         } else if (k->spaceMode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                    TupBackground *bg = currentScene->scene()->background();
                                    if (bg) {
                                        TupFrame *frame = bg->dynamicFrame();
                                        if (frame) {
                                            itemIndex = frame->indexOf(item);
                                        } else {
                                            #ifdef K_DEBUG
                                                   tError() << "TupPaintArea::deleteItems() - Fatal Error: Background frame is NULL!";
                                            #endif
                                        }
                                    } else {
                                        #ifdef K_DEBUG
                                               tError() << "TupPaintArea::deleteItems() - Fatal Error: Scene has no background element!";
                                        #endif
                                    }
                         } else {
                             #ifdef K_DEBUG
                                    tError() << "TupPaintArea::deleteItems() - Fatal Error: invalid spaceMode!";
                             #endif
                         }
                     }

                     if (itemIndex >= 0) {
                         TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                                  currentScene->currentSceneIndex(), currentScene->currentLayerIndex(), 
                                                  currentScene->currentFrameIndex(), 
                                                  itemIndex, QPointF(), k->spaceMode, type,
                                                  TupProjectRequest::Remove);
                         emit requestTriggered(&event);
                     } else {
                         #ifdef K_DEBUG
                                tFatal() << "TupPaintArea::deleteItems() - Fatal Error: Invalid item index";
                         #endif
                     }

                     counter++;
            }

        }
    }
}

void TupPaintArea::groupItems()
{
    // T_FUNCINFO;
    QList<QGraphicsItem *> selected = scene()->selectedItems();

    if (!selected.isEmpty()) {
        QString strItems= "";
        TupGraphicsScene *currentScene = graphicsScene();
        int firstItem = -1;

        if (currentScene) {
            foreach (QGraphicsItem *item, selected) {
                     if (currentScene->currentFrame()->indexOf(item) != -1) {
                         if (strItems.isEmpty()) {
                             strItems +="("+ QString::number(currentScene->currentFrame()->indexOf(item));
                             firstItem = currentScene->currentFrame()->indexOf(item);
                         } else {
                             strItems += " , " + 
                             QString::number(currentScene->currentFrame()->indexOf(item));
                         }
                     }
            }
            strItems+= ")";
        }

        if (strItems != ")") {
            TupProjectRequest event = TupRequestBuilder::createItemRequest(currentScene->currentSceneIndex(), 
                                     currentScene->currentLayerIndex(),
                                     currentScene->currentFrameIndex(), firstItem, QPointF(), k->spaceMode,
                                     TupLibraryObject::Item, TupProjectRequest::Group, strItems);
            emit requestTriggered(&event);
        }
    }
}

void TupPaintArea::ungroupItems()
{
    QList<QGraphicsItem *> selected = scene()->selectedItems();
    if (!selected.isEmpty()) {
        TupGraphicsScene* currentScene = graphicsScene();
        if (currentScene) {
            foreach (QGraphicsItem *item, selected) {
                     TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                              currentScene->currentSceneIndex(), 
                                              currentScene->currentLayerIndex(), 
                                              currentScene->currentFrameIndex(), 
                                              currentScene->currentFrame()->indexOf(item), QPointF(), 
                                              k->spaceMode, TupLibraryObject::Item,
                                              TupProjectRequest::Ungroup);
                     emit requestTriggered(&event);
            }
        }
    }
}

void TupPaintArea::copyItems()
{
    #ifdef K_DEBUG
           T_FUNCINFOX("paintarea");
    #endif

    k->copiesXml.clear();
    QList<QGraphicsItem *> selected = scene()->selectedItems();

    if (!selected.isEmpty()) {
        TupGraphicsScene* currentScene = graphicsScene();

        if (currentScene) {
            k->oldPosition = selected.at(0)->boundingRect().topLeft();
            foreach (QGraphicsItem *item, selected) {

                     QDomDocument dom;
                     dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));
                     k->copiesXml << dom.toString();

                     // Paint it to clipbard
                     QPixmap toPixmap(item->boundingRect().size().toSize());
                     toPixmap.fill(Qt::transparent);

                     QPainter painter(&toPixmap);
                     painter.setRenderHint(QPainter::Antialiasing);

                     QStyleOptionGraphicsItem opt;
                     opt.state = QStyle::State_None;

                     if (item->isEnabled())
                         opt.state |= QStyle::State_Enabled;
                     if (item->hasFocus())
                         opt.state |= QStyle::State_HasFocus;
                     if (item == currentScene->mouseGrabberItem())
                         opt.state |= QStyle::State_Sunken;

                     opt.exposedRect = item->boundingRect();
                     opt.levelOfDetail = 1;
                     opt.matrix = item->sceneMatrix();
                     opt.palette = palette();

                     item->paint(&painter, &opt, this);
                     painter.end();

                     QApplication::clipboard()->setPixmap(toPixmap);
            }
        }
    }
}

void TupPaintArea::pasteItems()
{
    #ifdef K_DEBUG
           T_FUNCINFOX("paintarea");
    #endif

    TupGraphicsScene* currentScene = graphicsScene();

    if (!k->menuOn) {
        k->position = viewPosition();
    }
    
    // QPointF point = k->position - k->oldPosition;

    foreach (QString xml, k->copiesXml) {
             TupLibraryObject::Type type = TupLibraryObject::Item;
             int total = currentScene->currentFrame()->graphicItemsCount();

             if (xml.startsWith("<svg")) {
                 type = TupLibraryObject::Svg;
                 total = currentScene->currentFrame()->svgItemsCount();
                 tError() << "TupPaintArea::pasteItems() - Pasting a SVG file...";
             }

             /*
             if (xml.startsWith("<ellipse")) {
                 tFatal() << "TupPaintArea::pasteItems() - Tracing ellipse!"; 
             } else {
                 tFatal() << "TupPaintArea::pasteItems() - NO ellipse detected!: " << xml;
             }
             */

             TupProjectRequest event = TupRequestBuilder::createItemRequest(currentScene->currentSceneIndex(),
                                      currentScene->currentLayerIndex(),
                                      currentScene->currentFrameIndex(),
                                      total, QPoint(), k->spaceMode, type,   
                                      TupProjectRequest::Add, xml);
             emit requestTriggered(&event);

             /* SQA: Pay attention to the point variable - Copy/Paste issue 
             TupProjectRequest event = TupRequestBuilder::createItemRequest(currentScene->currentSceneIndex(),
                                      currentScene->currentLayerIndex(), 
                                      currentScene->currentFrameIndex(), 
                                      total, point, k->spaceMode, type, 
                                      TupProjectRequest::Add, xml);
             emit requestTriggered(&event);
             */
     }

     k->menuOn = false;
}

void TupPaintArea::multipasteObject(int pasteTotal)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("paintarea");
    #endif

    TupGraphicsScene* currentScene = graphicsScene();

    if (!k->menuOn) {
        k->position = viewPosition();
    }
   
    foreach (QString xml, k->copiesXml) {
             TupLibraryObject::Type type = TupLibraryObject::Item;
             int total = currentScene->currentFrame()->graphicItemsCount();

             if (xml.startsWith("<svg")) {
                 type = TupLibraryObject::Svg;
                 total = currentScene->currentFrame()->svgItemsCount();
                 tError() << "TupPaintArea::pasteItems() - Pasting a SVG file...";
             }

             TupScene *scene = k->project->scene(currentScene->currentSceneIndex());
             if (scene) {
                 int framesTotal = scene->framesTotal();
                 int currentFrame = currentScene->currentFrameIndex();
                 int newFrameIndex = currentFrame + pasteTotal;
                 int distance = framesTotal - (newFrameIndex + 1);
                 
                 if (distance < 0) {
                     for (int i=framesTotal; i<=newFrameIndex; i++) {
                          TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentSceneIndex,
                                                                                            currentScene->currentLayerIndex(),
                                                                                            i,
                                                                                            TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
                          emit requestTriggered(&request);
                     }
                 }

                 int limit = currentFrame + pasteTotal;
                 for (int i=currentFrame+1; i<=limit; i++) {
                      TupProjectRequest event = TupRequestBuilder::createItemRequest(k->currentSceneIndex,
                                                                                     currentScene->currentLayerIndex(),
                                                                                     i,
                                                                                     total, QPoint(), k->spaceMode, type,
                                                                                     TupProjectRequest::Add, xml);
                      emit requestTriggered(&event);
                 }
             }
     }

     k->menuOn = false;
}

void TupPaintArea::pasteNextFive()
{
     multipasteObject(5);
}

void TupPaintArea::pasteNextTen()
{
     multipasteObject(10);
}

void TupPaintArea::pasteNextTwenty()
{
     multipasteObject(20);
}

void TupPaintArea::pasteNextFifty()
{
     multipasteObject(50);
}

void TupPaintArea::pasteNextHundred()
{
     multipasteObject(100);
}

void TupPaintArea::cutItems()
{
    #ifdef K_DEBUG
           T_FUNCINFOX("paintarea");
    #endif
    copyItems();
    deleteItems();
}

void TupPaintArea::setNextFramesOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    if (TupGraphicsScene* currentScene = graphicsScene())
        currentScene->setNextOnionSkinCount(n);
}

void TupPaintArea::setPreviousFramesOnionSkinCount(int n)
{
    #ifdef K_DEBUG
       T_FUNCINFO;
    #endif

    if (TupGraphicsScene* currentScene = graphicsScene())
        currentScene->setPreviousOnionSkinCount(n);
}

void TupPaintArea::addSelectedItemsToLibrary()
{
    #ifdef K_DEBUG
           tDebug("paintarea") << "Adding to library";
    #endif

    QList<QGraphicsItem *> selected = scene()->selectedItems();

    if (selected.isEmpty()) {
        TOsd::self()->display(tr("Error"), tr("No items selected"), TOsd::Error);
        return;
    }

    TupLibraryDialog dialog;
    foreach (QGraphicsItem *item, selected)
             dialog.addItem(item);
	
    if (dialog.exec() != QDialog::Accepted)
        return;

    foreach (QGraphicsItem *item, selected) {
             if (TupAbstractSerializable *itemSerializable = dynamic_cast<TupAbstractSerializable *>(item)) {
                 QString symName = dialog.symbolName(item) + ".obj";

                 QDomDocument doc;
                 doc.appendChild(itemSerializable->toXml(doc));

                 TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, 
                                            symName, TupLibraryObject::Item, k->spaceMode, doc.toString().toLocal8Bit(), QString());
                 emit requestTriggered(&request);
             }
    }
}

void TupPaintArea::requestItemMovement(QAction *action)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("paintarea");
    #endif

    QList<QGraphicsItem *> selected = scene()->selectedItems();

    if (selected.isEmpty()) {
        TOsd::self()->display(tr("Error"), tr("No items selected"), TOsd::Error);
        return;
    }
	
    TupGraphicsScene *currentScene = graphicsScene();
    if (!currentScene)
        return;

    TupFrame *currentFrame = currentScene->currentFrame();

    foreach (QGraphicsItem *item, selected) {
             TupLibraryObject::Type type = TupLibraryObject::Item; 
             int index = -1;
             if (TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item)) {
                 type = TupLibraryObject::Svg;
                 index = currentFrame->indexOf(svg);
             } else {
                 index = currentFrame->indexOf(item);
             }

             if (index >= 0) {
                 bool ok;
                 int moveType = action->data().toInt(&ok);

                 if (ok) {
                     TupProjectRequest event = TupRequestBuilder::createItemRequest(currentScene->currentSceneIndex(),
                                               currentScene->currentLayerIndex(), currentScene->currentFrameIndex(), index, QPointF(), 
                                               k->spaceMode, type, TupProjectRequest::Move, moveType);
                     emit requestTriggered(&event);
                 } else {
                     #ifdef K_DEBUG
                            tError() << "TupPaintArea::requestItemMovement() - Fatal error: Invalid action [ " << moveType << " ]";
                     #endif
                 }
             } else {
                 #ifdef K_DEBUG
                        tError() << "TupPaintArea::requestItemMovement() - Fatal error: Invalid object index [ " << index << " ]";
                 #endif
             }
    }
}

void TupPaintArea::updatePaintArea() 
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->spaceMode == TupProject::FRAMES_EDITION) {
        TupGraphicsScene* currentScene = graphicsScene();
        currentScene->drawCurrentPhotogram();
    } else {
        paintBackground();
    }
}

void TupPaintArea::paintBackground()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupGraphicsScene* currentScene = graphicsScene();
    currentScene->cleanWorkSpace();
    currentScene->drawBackground(currentScene->currentFrameIndex());
}

void TupPaintArea::setCurrentTool(QString tool) 
{
    #ifdef K_DEBUG
           T_FUNCINFO;
           SHOW_VAR(tool);
    #endif

    k->currentTool = tool;
    k->canvasEnabled = true;
}

void TupPaintArea::updateSpaceContext()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupGraphicsScene* currentScene = graphicsScene();
    currentScene->setSpaceMode(k->project->spaceContext());
    k->spaceMode = k->project->spaceContext();
}

void TupPaintArea::setOnionFactor(double value)
{
    TupGraphicsScene* currentScene = graphicsScene();
    currentScene->setOnionFactor(value);
}

void TupPaintArea::keyPressEvent(QKeyEvent *event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    #ifdef K_DEBUG
           tWarning() << "TupPaintArea::keyPressEvent() - Current tool: " << k->currentTool;
           tWarning() << "TupPaintArea::keyPressEvent() - Key: " << event->key();
    #endif

    if (k->currentTool.compare(tr("PolyLine")) == 0) {
        if (event->key() == Qt::Key_X)
            emit closePolyLine();
        return;
    }

    if (k->currentTool.compare(tr("Rectangle")) == 0 
        || k->currentTool.compare(tr("Ellipse")) == 0 
        || k->currentTool.compare(tr("Line")) == 0) {
        TupPaintAreaBase::keyPressEvent(event);
        return;
    }

    QList<QGraphicsItem *> selected = scene()->selectedItems();
    if (k->currentTool.compare(tr("Pencil")) != 0) {
        if (k->currentTool.compare(tr("Object Selection")) == 0) {
            if (!selected.isEmpty()) {
                TupPaintAreaBase::keyPressEvent(event);
                return;
            }
        }
    }

    // if (event->key() == Qt::Key_Left) {
    if (event->key() == Qt::Key_PageUp) {
        goOneFrameBack();
        return;
    }

    // if (event->key() == Qt::Key_Right) {
    if (event->key() == Qt::Key_PageDown) {
        if (event->modifiers() == Qt::ControlModifier)
            quickCopy();
        else 
            goOneFrameForward();
    }
}

void TupPaintArea::keyReleaseEvent(QKeyEvent *event)
{
    TupPaintAreaBase::keyReleaseEvent(event);
}

void TupPaintArea::goOneFrameBack()
{
    TupGraphicsScene *scene = graphicsScene();

    if (scene->currentFrameIndex() > 0) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene->currentSceneIndex(),
                                                                        scene->currentLayerIndex(),
                                                                        scene->currentFrameIndex() - 1,
                                                                        TupProjectRequest::Select, "1");
        emit requestTriggered(&request);
    }
}

void TupPaintArea::goOneFrameForward()
{
    TupGraphicsScene *scene = graphicsScene();
    int framesTotal = scene->framesTotal();
    int frameIndex = scene->currentFrameIndex() + 1;

    if (frameIndex == framesTotal) {
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene->currentSceneIndex(),
                                                     scene->currentLayerIndex(),
                                                     frameIndex,
                                                     TupProjectRequest::Add, tr("Frame %1").arg(frameIndex + 1));
        emit requestTriggered(&request);
    }

    goToFrame(scene->currentFrameIndex() + 1);
}

void TupPaintArea::copyCurrentFrame()
{
    TupGraphicsScene *gScene = graphicsScene();

    TupScene *scene = k->project->scene(gScene->currentSceneIndex());
    if (scene) {
        TupLayer *layer = scene->layer(gScene->currentLayerIndex());
        if (layer) {
            TupFrame *frame = layer->frame(gScene->currentFrameIndex());
            if (frame) {
                QDomDocument doc;
                doc.appendChild(frame->toXml(doc));
                k->frameCopy = doc.toString(0);
            }
        }
    }

}

void TupPaintArea::pasteDataOnCurrentFrame()
{
    TupGraphicsScene *scene = graphicsScene();


    TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene->currentSceneIndex(),
                                                                    scene->currentLayerIndex(), 
                                                                    scene->currentFrameIndex(),
                                                                    TupProjectRequest::Paste, k->frameCopy);
    emit requestTriggered(&request);
}

void TupPaintArea::quickCopy()
{
    copyCurrentFrame();
    goOneFrameForward();
    pasteDataOnCurrentFrame();
}

void TupPaintArea::goToFrame(int index)
{
    TupGraphicsScene *scene = graphicsScene();
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(scene->currentSceneIndex(),
                                                                    scene->currentLayerIndex(),
                                                                    index,
                                                                    TupProjectRequest::Select, "1");
    // emit requestTriggered(&request);
    emit localRequestTriggered(&request);
}

void TupPaintArea::goToFrame(int frameIndex, int layerIndex, int sceneIndex)
{
    TupProjectRequest request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                                                                      TupProjectRequest::Select, "1");
    // emit requestTriggered(&request);
    emit localRequestTriggered(&request);
}

void TupPaintArea::goToScene(int sceneIndex)
{
    TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
    // emit requestTriggered(&request);
    emit localRequestTriggered(&request);
}

/*
void TupPaintArea::tabletEvent(QTabletEvent *event)
{
    tError() << "TupPaintArea::tabletEvent() - Pressure: " << event->pressure();
    tError() << "TupPaintArea::tabletEvent() - xTilt: " << event->xTilt();
    tError() << "TupPaintArea::tabletEvent() - yTilt: " << event->yTilt();

    TupPaintAreaBase::tabletEvent(event);
}
*/

int TupPaintArea::currentSceneIndex()
{
    return graphicsScene()->currentSceneIndex();
}

int TupPaintArea::currentLayerIndex()
{
    return graphicsScene()->currentLayerIndex();
}
