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

#include "selectiontool.h"
#include "taction.h"
#include "talgorithm.h"
#include "tupgraphicalgorithm.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupsvgitem.h"
#include "tupsvg2qt.h"
#include "tupitemgroup.h"
#include "tupellipseitem.h"
#include "tupgraphicobject.h"
#include "tupinputdeviceinformation.h"
#include "tupgraphicsscene.h"
#include "nodemanager.h"
#include "tupserializer.h"
#include "tuprequestbuilder.h"
#include "tupprojectresponse.h"

struct SelectionTool::Private
{
    QMap<QString, TAction *> actions;
    QList<QGraphicsItem *> selectedObjects;
    QList<NodeManager*> nodeManagers;
    TupGraphicsScene *scene;
    bool selectionFlag;
    qreal scaleFactor;
    qreal realFactor;
    int baseZValue;
    TupEllipseItem *center;
    QGraphicsLineItem *target1;
    QGraphicsLineItem *target2;
    bool targetIsIncluded;
    QString key;
};

SelectionTool::SelectionTool(): k(new Private), panel(0)
{
    setupActions();
}

SelectionTool::~SelectionTool()
{
    delete k;
}

void SelectionTool::init(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[SelectionTool::init()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif
 
    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();
    k->scene = scene;
    k->scene->clearSelection();
    k->baseZValue = 20000 + (scene->scene()->layersTotal() * 10000);
    k->targetIsIncluded = false;

    reset(scene);
}

void SelectionTool::reset(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[SelectionTool::reset()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    int zBottomLimit = (scene->currentLayerIndex() + 2)*10000;
    int zTopLimit = zBottomLimit + 10000;

    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      // SQA: Temporary code for debugging issues
                      /*
                      QDomDocument dom;
                      dom.appendChild(dynamic_cast<TupAbstractSerializable *>(item)->toXml(dom));
                      QDomElement root = dom.documentElement();
                      tFatal() << "SelectionTool::init() - XML: ";
                      tFatal() << dom.toString();
                      */

                      int zValue = item->zValue();
                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if ((zValue >= zBottomLimit) && (zValue < zTopLimit) && (item->toolTip().length()==0)) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                         if (zValue >= 10000) {
                                             item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                                         } else {
                                             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                             item->setFlag(QGraphicsItem::ItemIsMovable, false);
                                         }
                              } else {
                                  #ifdef K_DEBUG
                                      QString msg = "SelectionTool::reset() - Fatal Error: Invalid spaceMode!";
                                      #ifdef Q_OS_WIN32
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                                  return;
                              }
                          }
                      }
             }
    }

    panel->enablePositionControls(false);
    if (k->targetIsIncluded) {
        k->scene->removeItem(k->center);
        k->scene->removeItem(k->target1);
        k->scene->removeItem(k->target2);
        k->targetIsIncluded = false;
    }
}

QStringList SelectionTool::keys() const
{
    return QStringList() << tr("Selection");
}

void SelectionTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    k->selectionFlag = false;

    // If Control key is pressed / allow multiple selection picking items one by one 
    if (input->keyModifiers() != Qt::ControlModifier) {
        foreach (NodeManager *nodeManager, k->nodeManagers) {
                 if (!nodeManager->isPress()) {
                     nodeManager->parentItem()->setSelected(false);
                     k->nodeManagers.removeAll(nodeManager);
                     scene->drawCurrentPhotogram();
                 } 
        }
        k->selectedObjects.clear();
    }

    if (scene->currentFrame()->indexOf(scene->mouseGrabberItem()) != -1) {
        k->selectedObjects << scene->mouseGrabberItem();
    } else {
        if (scene->selectedItems().count() > 0)
            k->selectedObjects = scene->selectedItems();
    }

    foreach (QGraphicsItem *item, k->selectedObjects) {
             if (item && (dynamic_cast<TupAbstractSerializable* > (item))) {
                 if (item->group() != 0)
                     item = qgraphicsitem_cast<QGraphicsItem *>(item->group());
                 bool found = false;
                 foreach (NodeManager *nodeManager, k->nodeManagers) {
                          if (item == nodeManager->parentItem()) {
                              found = true;
                              break;
                          }
                 }
            
                 if (!found) {
                     NodeManager *manager = new NodeManager(item, scene, k->baseZValue);
                     manager->show();
                     manager->resizeNodes(k->realFactor);
                     k->nodeManagers << manager;
                 }
             } 
    }
}

void SelectionTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    if (input->buttons() == Qt::LeftButton && scene->selectedItems().count() > 0)
        QTimer::singleShot(0, this, SLOT(syncNodes()));
}

void SelectionTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    k->selectedObjects = scene->selectedItems();

    if (k->selectedObjects.count() > 0) {
        k->selectionFlag = true;

        foreach (NodeManager *node, k->nodeManagers) {
                 QGraphicsItem *item = node->parentItem();
                 int parentIndex = k->selectedObjects.indexOf(item); 
                 if (parentIndex != -1) // Object is IN the list
                     k->selectedObjects.removeAt(parentIndex); // Remove node's item from selected objects list 
                 else // Object is NOT IN the selected objects list
                     delete k->nodeManagers.takeAt(k->nodeManagers.indexOf(node)); // Removing node manager from nodes list 
        }


        foreach (QGraphicsItem *item, k->selectedObjects) {
                 if (item && (dynamic_cast<TupAbstractSerializable* > (item))) {
                     if (item->group() != 0)
                         item = qgraphicsitem_cast<QGraphicsItem *>(item->group());
                     bool found = false;
                     foreach (NodeManager *nodeManager, k->nodeManagers) {
                              if (item == nodeManager->parentItem()) {
                                  found = true;
                                  break;
                              }
                     }

                     if (!found) {
                         NodeManager *manager = new NodeManager(item, scene, k->baseZValue);
                         manager->show();
                         manager->resizeNodes(k->realFactor);
                         k->nodeManagers << manager;
                     }
                 }
        }

        foreach (NodeManager *node, k->nodeManagers) {
                 if (node->isModified()) {
                     QGraphicsItem *item = node->parentItem();
                     QDomDocument doc;
                     doc.appendChild(TupSerializer::properties(item, doc));
                     int position = -1;
                     TupLibraryObject::Type type;
                     TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);

                     if (svg) {
                         type = TupLibraryObject::Svg;
                         if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                             position = scene->currentFrame()->indexOf(svg);
                         } else {
                             TupBackground *bg = k->scene->scene()->background();
                             if (bg) {
                                 if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                     position = bg->staticFrame()->indexOf(svg);
                                 } else if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                     position = bg->dynamicFrame()->indexOf(svg);
                                 } else {
                                     #ifdef K_DEBUG
                                         QString msg = "SelectionTool::release() - Fatal Error: Invalid spaceMode!";
                                         #ifdef Q_OS_WIN32
                                             qDebug() << msg;
                                         #else
                                             tError() << msg;
                                         #endif
                                     #endif
                                     return;
                                 }
                             } else {
                                 #ifdef K_DEBUG
                                     QString msg = "SelectionTool::release() - Fatal Error: Scene background object is NULL!";
                                     #ifdef Q_OS_WIN32
                                         qDebug() << msg;
                                     #else
                                         tError() << msg;
                                     #endif
                                 #endif
                                 return;
                             }
                         }
                     } else {
                         type = TupLibraryObject::Item;

                         if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                             position = scene->currentFrame()->indexOf(node->parentItem());
                         } else {
                             TupBackground *bg = scene->scene()->background();
                             if (bg) {
                                 if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                     position = bg->staticFrame()->indexOf(node->parentItem());
                                 } else if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                            position = bg->dynamicFrame()->indexOf(node->parentItem());
                                 } else {
                                     #ifdef K_DEBUG
                                         QString msg = "SelectionTool::release() - Fatal Error: Invalid spaceMode!";
                                         #ifdef Q_OS_WIN32
                                             qDebug() << msg;
                                         #else
                                             tError() << msg;
                                         #endif
                                     #endif
                                     return;
                                 }
                             } else {
                                 #ifdef K_DEBUG
                                     QString msg = "SelectionTool::release() - Fatal Error: Scene background object is NULL!";
                                     #ifdef Q_OS_WIN32
                                         qDebug() << msg;
                                     #else
                                         tError() << msg;
                                     #endif
                                 #endif
                                 return;
                             }
                         }
                     }

                     if (position >= 0) {
                         // Restore matrix
                         // node->restoreItem();

                         TupProjectRequest event = TupRequestBuilder::createItemRequest( 
                                    scene->currentSceneIndex(), 
                                    scene->currentLayerIndex(), 
                                    scene->currentFrameIndex(), position, QPointF(), 
                                    scene->spaceMode(), type,
                                    TupProjectRequest::Transform, doc.toString());

                         emit requested(&event);
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "SelectionTool::release() - Fatal Error: Invalid item position !!! [ " + QString::number(position) + " ]";
                             #ifdef Q_OS_WIN32
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                     }
                 }
        }
        updateItemPosition();
    } else {
        panel->enablePositionControls(false);
        if (k->targetIsIncluded)
            k->targetIsIncluded = false;

        foreach (NodeManager *nodeManager, k->nodeManagers) {
                 nodeManager->parentItem()->setSelected(false);
                 k->nodeManagers.removeAll(nodeManager);
        }

        scene->drawCurrentPhotogram();
    }
}

void SelectionTool::setupActions()
{
    k->selectionFlag = false;
    k->scaleFactor = 1;
    k->realFactor = 1;

    TAction *select = new TAction(QPixmap(kAppProp->themeDir() + "icons/selection.png"), tr("Object Selection"), this);
    select->setShortcut(QKeySequence(tr("O")));
    select->setToolTip(tr("Object Selection") + " - " + "O");

    k->actions.insert(tr("Selection"), select);
}

QMap<QString, TAction *> SelectionTool::actions() const
{
    return k->actions;
}

int SelectionTool::toolType() const
{
    return TupToolInterface::Selection;
}

QWidget *SelectionTool::configurator() 
{
    if (!panel) {
        panel = new Settings;
        connect(panel, SIGNAL(callFlip(Settings::Flip)), this, SLOT(applyFlip(Settings::Flip)));
        connect(panel, SIGNAL(callOrderAction(Settings::Order)), this, SLOT(applyOrderAction(Settings::Order)));
        connect(panel, SIGNAL(callGroupAction(Settings::Group)), this, SLOT(applyGroupAction(Settings::Group)));
        connect(panel, SIGNAL(updateItemPosition(int, int)), this, SLOT(updateItemPosition(int, int)));
    }

    return panel;
}

void SelectionTool::aboutToChangeScene(TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[SelectionTool::aboutToChangeScene()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    init(scene);
}

void SelectionTool::aboutToChangeTool()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[SelectionTool::aboutToChangeTool()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    qDeleteAll(k->nodeManagers);
    k->nodeManagers.clear();

    foreach (QGraphicsView *view, k->scene->views()) {
             view->setDragMode (QGraphicsView::NoDrag);
             foreach (QGraphicsItem *item, view->scene()->items()) {
                      item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                      item->setFlag(QGraphicsItem::ItemIsMovable, false);
             }
    }
}

void SelectionTool::itemResponse(const TupItemResponse *event)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[SelectionTool::itemResponse()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    QGraphicsItem *item = 0;
    TupScene *scene = 0;
    TupLayer *layer = 0;
    TupFrame *frame = 0;

    TupProject *project = k->scene->scene()->project();
    
    if (project) {
        scene = project->scene(event->sceneIndex());
        if (scene) {
            if (project->spaceContext() == TupProject::FRAMES_EDITION) {
                layer = scene->layer(event->layerIndex());

                if (layer) {
                    frame = layer->frame(event->frameIndex());
                    if (frame) {
                        if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
                            item = frame->svg(event->itemIndex());
                        } else if (frame->graphicItemsCount()>0) {
                                   item = frame->item(event->itemIndex());
                        }
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "SelectionTool::itemResponse() - Fatal Error: frame is NULL";
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                        return;
                    }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "SelectionTool::itemResponse() - Fatal Error: layer is NULL";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return;
                }
            } else if (project->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                       TupBackground *bg = scene->background();
                       if (bg) {
                           TupFrame *frame = bg->staticFrame();
                           if (frame) {
                               if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
                                   item = frame->svg(event->itemIndex());
                               } else if (frame->graphicItemsCount()>0) {
                                          item = frame->item(event->itemIndex());
                               }
                           } else {
                               #ifdef K_DEBUG
                                   QString msg = "SelectionTool::itemResponse() - Fatal Error: Static bg frame is NULL";
                                   #ifdef Q_OS_WIN32
                                       qDebug() << msg;
                                   #else
                                       tError() << msg;
                                   #endif
                               #endif
                               return;
                           }
                       } else {
                           #ifdef K_DEBUG
                               QString msg = "SelectionTool::itemResponse() - Fatal Error: Scene background is NULL";
                               #ifdef Q_OS_WIN32
                                   qDebug() << msg;
                               #else
                                   tError() << msg;
                               #endif
                           #endif
                           return;
                       }
            } else if (project->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                       TupBackground *bg = scene->background();
                       if (bg) {
                           TupFrame *frame = bg->dynamicFrame();
                           if (frame) {
                               if (event->itemType() == TupLibraryObject::Svg && frame->svgItemsCount()>0) {
                                   item = frame->svg(event->itemIndex());
                               } else if (frame->graphicItemsCount()>0) {
                                          item = frame->item(event->itemIndex());
                               }
                           } else {
                               #ifdef K_DEBUG
                                   QString msg = "SelectionTool::itemResponse() - Fatal Error: Dynamic bg frame is NULL";
                                   #ifdef Q_OS_WIN32
                                       qDebug() << msg;
                                   #else
                                       tError() << msg;
                                   #endif
                               #endif
                               return;
                           }
                       } else {
                           #ifdef K_DEBUG
                               QString msg = "SelectionTool::itemResponse() - Fatal Error: Scene background is NULL";
                               #ifdef Q_OS_WIN32
                                   qDebug() << msg;
                               #else
                                   tError() << msg;
                               #endif
                           #endif
                           return;
                       }
            } else {
                #ifdef K_DEBUG
                    QString msg = "SelectionTool::itemResponse() - Fatal Error: Invalid spaceMode!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }
        } else {
            #ifdef K_DEBUG
                QString msg = "SelectionTool::itemResponse() - Fatal Error: Current scene is NULL!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "SelectionTool::itemResponse() - Project does not exist";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }

    updateItemPosition();

    switch (event->action()) {
            case TupProjectRequest::Transform:
            {
                 if (item) {
                     foreach (NodeManager* node, k->nodeManagers) {
                              node->show();
                              node->syncNodesFromParent();
                              node->beginToEdit();
                     }
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "SelectionTool::itemResponse - No item found";
                         #ifdef Q_OS_WIN32
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                 }
            }
            break;
            case TupProjectRequest::Move:
            {
                 syncNodes();
            }
            break;
            case TupProjectRequest::Group:
            {
                 k->nodeManagers.clear();
                 k->selectedObjects.clear();

                 k->selectedObjects << item;
                 item->setSelected(true);
                 NodeManager *node = new NodeManager(item, k->scene, k->baseZValue);
                 node->resizeNodes(k->realFactor);
                 k->nodeManagers << node;

                 syncNodes();
            }
            break;
            case TupProjectRequest::Ungroup:
            {
                 foreach (QGraphicsItem *graphic, k->scene->selectedItems())
                          graphic->setSelected(false);

                 k->nodeManagers.clear();
                 k->selectedObjects.clear();

                 QString list = event->arg().toString();
                 QString::const_iterator itr = list.constBegin();
                 QList<int> positions = TupSvg2Qt::parseIntList(++itr);
                 qSort(positions.begin(), positions.end());
                 int total = positions.size();
                 for (int i=0; i<total; i++) {
                      QGraphicsItem *graphic = frame->item(positions.at(i));     
                      if (graphic) {
                          k->selectedObjects << graphic;
                          graphic->setSelected(true);
                          NodeManager *node = new NodeManager(graphic, k->scene, k->baseZValue);
                          node->resizeNodes(k->realFactor);
                          k->nodeManagers << node;
                      }
                 }

                 syncNodes();
            }
            break;
            case TupProjectRequest::Remove:
            {
                 // Do nothing
            }
            break;
            default:
            {
                 syncNodes();
            }
            break;
    }
}

void SelectionTool::syncNodes()
{
    foreach (NodeManager* node, k->nodeManagers) {
             if (node) {
                 node->show();
                 if (node->parentItem()) {
                     // node->parentItem()->setSelected(true);
                     node->syncNodesFromParent();
                 }
             }
    }
}

void SelectionTool::saveConfig()
{
}

void SelectionTool::keyPressEvent(QKeyEvent *event)
{
    k->key = "NONE";

    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    } else if (event->modifiers() == Qt::AltModifier) {
               if (event->key() == Qt::Key_R) {
                   if (selectionIsActive()) {
                       foreach (NodeManager *nodeManager, k->nodeManagers) {
                                nodeManager->toggleAction();
                                break;
                       }
                   }
               }
    } else if ((event->key() == Qt::Key_Left) || (event->key() == Qt::Key_Up) 
               || (event->key() == Qt::Key_Right) || (event->key() == Qt::Key_Down)) {

               if (!selectionIsActive()) {
                   QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
                   if (flags.first != -1 && flags.second != -1)
                       emit callForPlugin(flags.first, flags.second);
               } else {
                   int delta = 5;

                   if (event->modifiers()==Qt::ShiftModifier)
                       delta = 1;

                   if (event->modifiers()==Qt::ControlModifier)
                       delta = 10;

                   k->selectedObjects = k->scene->selectedItems();

                   foreach (QGraphicsItem *item, k->selectedObjects) {
                            if (event->key() == Qt::Key_Left)
                                item->moveBy(-delta, 0);

                            if (event->key() == Qt::Key_Up)
                                item->moveBy(0, -delta);

                            if (event->key() == Qt::Key_Right)
                                item->moveBy(delta, 0);

                            if (event->key() == Qt::Key_Down)
                                item->moveBy(0, delta);

                            QTimer::singleShot(0, this, SLOT(syncNodes()));
                   }

                   updateItemPosition();
               }
    } else if (event->modifiers() == Qt::ShiftModifier) {
               k->key = "SHIFT";
               if (selectionIsActive()) {
                   foreach (NodeManager *nodeManager, k->nodeManagers)
                            nodeManager->setProportion(true);
               }
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

void SelectionTool::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);

    if (k->key.compare("SHIFT") == 0) {
        if (selectionIsActive()) {
            foreach (NodeManager *nodeManager, k->nodeManagers)
                     nodeManager->setProportion(false);
        }
    }
}

bool SelectionTool::selectionIsActive()
{
    return k->selectionFlag;

    /*
    if (k->scene) {
        if (!k->selectionFlag)
            return;
    } else {
        return;
    }
    */
}

// SQA: Check if reset() and updateItems can be defined as the same method 
void SelectionTool::updateItems(TupGraphicsScene *scene)
{
    foreach (QGraphicsView *view, scene->views()) {
             view->setDragMode(QGraphicsView::RubberBandDrag);
             foreach (QGraphicsItem *item, scene->items()) {
                      if (!qgraphicsitem_cast<Node *>(item)) {
                          if (scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (item->zValue() >= 20000) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else {
                                  item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                  item->setFlag(QGraphicsItem::ItemIsMovable, false);
                              }
                          } else {
                              if (scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                  item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                              } else if (scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                         if (item->zValue() >= 10000) {
                                             item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                                         } else {
                                             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
                                             item->setFlag(QGraphicsItem::ItemIsMovable, false);
                                         }
                              } else {
                                  #ifdef K_DEBUG
                                      QString msg = "SelectionTool::updateItems() - Fatal Error: Invalid spaceMode!";
                                      #ifdef Q_OS_WIN32
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                              }
                          }
                      }
             }
    }
}

void SelectionTool::applyFlip(Settings::Flip flip)
{
    k->selectedObjects = k->scene->selectedItems();

    foreach (QGraphicsItem *item, k->selectedObjects) {
             QRectF rect = item->sceneBoundingRect();
             QPointF point = rect.topLeft();
             QMatrix m;
             m.translate(point.x(), point.y());

             if (flip == Settings::Horizontal)
                 m.scale(-1.0, 1.0);
             else if (flip == Settings::Vertical)
                      m.scale(1.0, -1.0);
             else if (flip == Settings::Crossed)
                      m.scale(-1.0, -1.0);

             m.translate(-point.x(), -point.y());
             item->setMatrix(m, true);
             rect = item->sceneBoundingRect();
             QPointF point2 = rect.topLeft();

             QPointF result = point - point2;
             item->moveBy(result.x(), result.y());

             foreach (NodeManager *node, k->nodeManagers) {
                      if (node->isModified()) {
                          QDomDocument doc;
                          doc.appendChild(TupSerializer::properties(item, doc));

                          TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(node->parentItem());
                          int position = -1;
                          TupLibraryObject::Type type = TupLibraryObject::Item;
                          if (svg)
                              type = TupLibraryObject::Svg;

                          if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                              if (svg) 
                                  position = k->scene->currentFrame()->indexOf(svg);
                              else 
                                  position = k->scene->currentFrame()->indexOf(node->parentItem());
                          } else {
                              TupBackground *bg = k->scene->scene()->background();
                              if (bg) {
                                  if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                                      if (svg)
                                          position = bg->staticFrame()->indexOf(svg);
                                      else
                                          position = bg->staticFrame()->indexOf(node->parentItem());
                                  } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                             if (svg)
                                                 position = bg->dynamicFrame()->indexOf(svg);
                                             else
                                                 position = bg->dynamicFrame()->indexOf(node->parentItem());
                                  } else {
                                      #ifdef K_DEBUG
                                          QString msg = "SelectionTool::applyFlip() - Fatal Error: invalid spaceMode!";
                                          #ifdef Q_OS_WIN32
                                              qDebug() << msg;
                                          #else
                                              tError() << msg;
                                          #endif
                                      #endif
                                      return;
                                  }
                              } else {
                                  #ifdef K_DEBUG
                                      QString msg = "SelectionTool::applyFlip() - Fatal Error: Scene background object is NULL!";
                                      #ifdef Q_OS_WIN32
                                          qDebug() << msg;
                                      #else
                                          tError() << msg;
                                      #endif
                                  #endif
                                  return;
                              }
                          }

                          TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                                   k->scene->currentSceneIndex(),
                                                   k->scene->currentLayerIndex(),
                                                   k->scene->currentFrameIndex(), position, QPointF(), 
                                                   k->scene->spaceMode(), type,
                                                   TupProjectRequest::Transform, doc.toString());
                          emit requested(&event);
                      }
             }
    }
}

void SelectionTool::applyOrderAction(Settings::Order action)
{
    k->selectedObjects = k->scene->selectedItems();

    foreach (QGraphicsItem *item, k->selectedObjects) {
             TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
             int position = -1;
             TupLibraryObject::Type type = TupLibraryObject::Item;
             if (svg)
                 type = TupLibraryObject::Svg;

             if (k->scene->spaceMode() == TupProject::FRAMES_EDITION) {
                 if (svg)
                     position = k->scene->currentFrame()->indexOf(svg);
                 else
                     position = k->scene->currentFrame()->indexOf(item);
             } else {
                 TupBackground *bg = k->scene->scene()->background();
                 if (bg) {
                     if (k->scene->spaceMode() == TupProject::STATIC_BACKGROUND_EDITION) {
                         if (svg)
                             position = bg->staticFrame()->indexOf(svg);
                         else
                             position = bg->staticFrame()->indexOf(item);
                     } else if (k->scene->spaceMode() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                                if (svg)
                                    position = bg->dynamicFrame()->indexOf(svg);
                                else
                                    position = bg->dynamicFrame()->indexOf(item);
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "SelectionTool::applyOrderAction() - Fatal Error: invalid spaceMode!";
                             #ifdef Q_OS_WIN32
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                         return;
                     }
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "SelectionTool::applyOrderAction() - Fatal Error: Scene background object is NULL!";
                         #ifdef Q_OS_WIN32
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                     return;
                 }
             }

             TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                       k->scene->currentLayerIndex(), k->scene->currentFrameIndex(), position, QPointF(),
                                       k->scene->spaceMode(), type, TupProjectRequest::Move, action);
             emit requested(&event);
    }
}

void SelectionTool::applyGroupAction(Settings::Group action)
{
    if (action == Settings::GroupItems) {
        k->selectedObjects = k->scene->selectedItems();
        int total = k->selectedObjects.count(); 
        if (total > 1) {
            QString items = "(";
            int i = 1;
            int position = -1; 

            foreach (QGraphicsItem *item, k->selectedObjects) {
                     int index = k->scene->currentFrame()->indexOf(item);
                     if (index > -1) {
                         if (i == 1) {
                             position = index;
                         } else {
                             if (index < position) 
                                 position = index;
                         }
                         items += QString::number(index); 
                         if (i < total)
                             items += ", ";
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "SelectionTool::applyGroupAction() - Fatal Error: Index of item is invalid! -> -1";
                             #ifdef Q_OS_WIN32
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                     }
                     i++;
            }
            items += ")";

            foreach (QGraphicsItem *item, k->selectedObjects)
                     item->setSelected(false);

            TupProjectRequest event = TupRequestBuilder::createItemRequest(k->scene->currentSceneIndex(),
                                      k->scene->currentLayerIndex(),
                                      k->scene->currentFrameIndex(), position, QPointF(), k->scene->spaceMode(),
                                      TupLibraryObject::Item, TupProjectRequest::Group, items);
            emit requested(&event);
        } else if (total == 1) {
                   k->nodeManagers.clear();
                   k->scene->drawCurrentPhotogram();
        }
    } else if (action == Settings::UngroupItems) {
               k->selectedObjects = k->scene->selectedItems();
               int total = k->selectedObjects.count();

               if (total > 0) {
                   bool noAction = true;
                   foreach (QGraphicsItem *item, k->selectedObjects) {
                            item->setSelected(false);
                            if (qgraphicsitem_cast<TupItemGroup *> (item)) {
                                noAction = false;
                                int position = k->scene->currentFrame()->indexOf(item);
                                TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                                          k->scene->currentSceneIndex(),
                                                          k->scene->currentLayerIndex(),
                                                          k->scene->currentFrameIndex(),
                                                          position, QPointF(),
                                                          k->scene->spaceMode(), TupLibraryObject::Item,
                                                          TupProjectRequest::Ungroup);
                                emit requested(&event);
                            }
                   }

                   if (noAction) {
                       k->nodeManagers.clear();
                       k->scene->drawCurrentPhotogram();
                   }
               }
    }
}

QCursor SelectionTool::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

void SelectionTool::resizeNodes(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
    foreach (NodeManager *manager, k->nodeManagers)
             manager->resizeNodes(scaleFactor);
}

void SelectionTool::updateZoomFactor(qreal scaleFactor)
{
    k->realFactor = scaleFactor;
}

void SelectionTool::sceneResponse(const TupSceneResponse *event)
{
    if (event->action() == TupProjectRequest::Select)
        reset(k->scene);
}

void SelectionTool::updateItemPosition() 
{
    if (k->nodeManagers.count() == 1) {
        NodeManager *manager = k->nodeManagers.first();
        QGraphicsItem *item = manager->parentItem();
        QPoint point = item->mapToScene(item->boundingRect().center()).toPoint();
        panel->setPos(point.x(), point.y());
    } else { 
        if (k->nodeManagers.count() > 1) {
            NodeManager *manager = k->nodeManagers.first();
            QGraphicsItem *item = manager->parentItem();
            QPoint left = item->mapToScene(item->boundingRect().topLeft()).toPoint();  
            QPoint right = item->mapToScene(item->boundingRect().bottomRight()).toPoint();
            int minX = left.x();
            int maxX = right.x(); 
            int minY = left.y();
            int maxY = right.y();

            foreach (NodeManager *node, k->nodeManagers) {
                     QGraphicsItem *item = node->parentItem();
                     QPoint left = item->mapToScene(item->boundingRect().topLeft()).toPoint(); 
                     int leftX = left.x();
                     int leftY = left.y();
                     if (leftX < minX)
                         minX = leftX;
                     if (leftY < minY)
                         minY = leftY;
                     QPoint right = item->mapToScene(item->boundingRect().bottomRight()).toPoint();  
                     int rightX = right.x();
                     int rightY = right.y();
                     if (rightX > maxX)
                         maxX = rightX;
                     if (rightY > maxY)
                         maxY = rightY;
            }
            int x = minX + ((maxX - minX)/2); 
            int y = minY + ((maxY - minY)/2);
            panel->setPos(x, y);

            if (!k->targetIsIncluded) {
                k->center = new TupEllipseItem(QRectF(QPointF(x - 1, y - 1), QSize(2, 2)));
                k->target1 = new QGraphicsLineItem(x, y - 6, x, y + 6);
                k->target2 = new QGraphicsLineItem(x - 6, y, x + 6, y);
              
                QPen pen(QColor(255, 0, 0), 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                k->center->setPen(pen);
                k->center->setBrush(QColor(255, 0, 0));
                k->center->setZValue(k->baseZValue + 1);
                k->scene->includeObject(k->center);

                k->target1->setPen(pen); 
                k->target1->setZValue(1000);
                k->target2->setPen(pen);
                k->target2->setZValue(1000);

                k->scene->includeObject(k->target1);
                k->scene->includeObject(k->target2);

                k->targetIsIncluded = true;
            } else {
                QPoint current = k->center->mapToScene(k->center->boundingRect().topLeft()).toPoint(); 
                int deltaX = x - current.x() - 1;
                int deltaY = y - current.y() - 1;
                k->center->moveBy(deltaX, deltaY);
                k->target1->moveBy(deltaX, deltaY);
                k->target2->moveBy(deltaX, deltaY);
            }
        }
    }
}

void SelectionTool::updateItemPosition(int x, int y) 
{
    if (k->nodeManagers.count() == 1) {
        NodeManager *manager = k->nodeManagers.first();
        QGraphicsItem *item = manager->parentItem();
        item->moveBy(x, y);
        manager->syncNodesFromParent();
    } else {
        if (k->nodeManagers.count() > 1) {
            foreach (NodeManager *node, k->nodeManagers) {
                     QGraphicsItem *item = node->parentItem();
                     item->moveBy(x, y);
                     node->syncNodesFromParent();
            }
            k->center->moveBy(x, y);
            k->target1->moveBy(x, y);
            k->target2->moveBy(x, y);
        }
    }
}
