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

#include "tupcommandexecutor.h"
#include "tupserializer.h"
#include "tupscene.h"

#include "tuppathitem.h"
#include "tuprectitem.h"
#include "tuplineitem.h"
#include "tupellipseitem.h"
#include "tupitemconverter.h"
#include "tupsvg2qt.h"

#include "tupprojectrequest.h"
#include "tuprequestbuilder.h"
#include "tupitemfactory.h"
#include "tupprojectresponse.h"
#include "tupproxyitem.h"
#include "tuptweenerstep.h"
#include "tupitemtweener.h"
#include "tupgraphicobject.h"
#include "tuplayer.h"
#include "tupbackground.h"

#include <QGraphicsItem>

bool TupCommandExecutor::createItem(TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::createItem()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif        
    
    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    TupLibraryObject::Type type = response->itemType(); 
    QPointF point = response->position();

    TupProject::Mode mode = response->spaceMode();
    QString xml = response->arg().toString();

    TupScene *scene = m_project->scene(scenePosition);
  
    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    if (type == TupLibraryObject::Svg) {
                        TupSvgItem *svg = frame->createSvgItem(point, xml);
                        if (svg) {
                            response->setItemIndex(frame->svgItemsCount()-1);
                        } else {                            
                            #ifdef K_DEBUG
                                QString msg = "TupCommandExecutor::createItem() - Error: Svg object is invalid!";
                                #ifdef Q_OS_WIN32
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif                            
                            return false;
                        }
                    } else {
                        QGraphicsItem *item = frame->createItem(point, xml);
                        if (item) {
                            response->setItemIndex(frame->graphicItemsCount()-1);
                        } else {
                            #ifdef K_DEBUG
                                QString msg = "TupCommandExecutor::createItem() - Error: QGraphicsItem object is invalid!";
                                #ifdef Q_OS_WIN32
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif    
                            return false;
                        }
                    }

                    response->setFrameState(frame->isEmpty());
                    emit responsed(response);

                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::createItem() - Error: Frame index doesn't exist! -> " + QString::number(framePosition);
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif    
                    return false;
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::createItem() - Error: Layer index doesn't exist! -> " + QString::number(layerPosition);
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }

        } else { 
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::createItem() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif    
                    return false;
                }

                if (frame) {
                    if (type == TupLibraryObject::Svg) {
                        TupSvgItem *svg = frame->createSvgItem(point, xml);
                        if (svg) {
                            response->setItemIndex(frame->indexOf(svg));
                        } else {
                            #ifdef K_DEBUG
                                QString msg = "TupCommandExecutor::createItem() - Error: Svg object is invalid!";
                                #ifdef Q_OS_WIN32
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif    
                            return false;
                        }
                    } else {
                        QGraphicsItem *item = frame->createItem(point, xml);
                        if (item) {
                            response->setItemIndex(frame->indexOf(item));
                        } else {
                            #ifdef K_DEBUG
                                QString msg = "TupCommandExecutor::createItem() - Error: QGraphicsItem object is invalid!";
                                #ifdef Q_OS_WIN32
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif    
                            return false;
                        }
                    }

                    emit responsed(response);
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::createItem() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif    
                    return false;
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::createItem() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif                
                return false;
            }
        }

    } else {
        #ifdef K_DEBUG
            QString msg = "TupCommandExecutor::createItem() - Error: Invalid scene index!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif    
        return false;
    }
    
    return true;
}

bool TupCommandExecutor::removeItem(TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::removeItem()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif    

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    TupLibraryObject::Type type = response->itemType();
    TupProject::Mode mode = response->spaceMode();

    TupScene *scene = m_project->scene(scenePosition);

    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);

            if (layer) {
                TupFrame *frame = layer->frame(framePosition);

                if (frame) {
                    if (type == TupLibraryObject::Svg) {
                        frame->removeSvgAt(response->itemIndex());

                        response->setFrameState(frame->isEmpty());
                        emit responsed(response);
                        return true;
                    } else {
                        TupGraphicObject *object = frame->graphic(response->itemIndex());

                        if (object) {
                            frame->removeGraphicAt(response->itemIndex());
                            if (object->hasTween()) 
                                scene->removeTweenObject(object);

                            response->setFrameState(frame->isEmpty());
                            emit responsed(response);
                            return true;

                        } else {
                            #ifdef K_DEBUG
                                QString msg = "TupCommandExecutor::removeItem() - Error: Invalid object index (value: " + QString::number(response->itemIndex()) + ")";
                                #ifdef Q_OS_WIN32
                                    qDebug() << msg;
                                #else
                                    tError() << msg;
                                #endif
                            #endif                                
                            return false;
                        }
                    }
                }
            }

        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::removeItem() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

                if (frame) {
                    if (type == TupLibraryObject::Svg) 
                        frame->removeSvgAt(response->itemIndex());
                    else
                        frame->removeGraphicAt(response->itemIndex());

                    emit responsed(response);
                    return true;
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::removeItem() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::removeItem() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }

    } else {
        #ifdef K_DEBUG
            QString msg = "TupCommandExecutor::removeItem() - Error: Invalid scene index!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return false;
    }
    
    return false;
}

bool TupCommandExecutor::moveItem(TupItemResponse *response)
{    
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::moveItem()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif    

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    int objectIndex = response->itemIndex();
    int action = response->arg().toInt();
    TupLibraryObject::Type type = response->itemType();
    TupProject::Mode mode = response->spaceMode();

    if (response->mode() == TupProjectResponse::Undo) {
        // SQA: Recalculate the variable values based on the action code 
        // objectIndex = ???;
        // action = ???;
    }
    
    TupScene *scene = m_project->scene(scenePosition);
    
    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    if (frame->moveItem(type, objectIndex, action)) {
                        emit responsed(response);
                        return true;
                    }
                }
            } 
        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::moveItem() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

                if (frame) {
                    if (frame->moveItem(type, objectIndex, action)) {
                        emit responsed(response);
                        return true;
                    }
                } else {                    
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::moveItem() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::moveItem() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }

    return false;
}


bool TupCommandExecutor::groupItems(TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::groupItems()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    int position = response->itemIndex();
    TupProject::Mode mode = response->spaceMode();
    QString strList = response->arg().toString();

    TupScene *scene = m_project->scene(scenePosition);
    
    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    QString::const_iterator itr = strList.constBegin();
                    QList<int> positions = TupSvg2Qt::parseIntList(++itr);
                    qSort(positions.begin(), positions.end());
                    int itemIndex = frame->createItemGroup(position, positions);
                    response->setItemIndex(itemIndex);
                
                    emit responsed(response);
                    return true;
                }
            }
        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::groupItems() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif                    
                    return false;
                }

                if (frame) {
                    QString::const_iterator itr = strList.constBegin();
                    QList<int> positions = TupSvg2Qt::parseIntList(++itr);
                    qSort(positions.begin(), positions.end());
                    int itemIndex = frame->createItemGroup(position, positions);
                    response->setItemIndex(itemIndex);

                    emit responsed(response);
                    return true;
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::groupItems() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif 
                    return false;
                }

            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::groupItems() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }
    
    return false;
}

bool TupCommandExecutor::ungroupItems(TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::ungroupItems()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    int position = response->itemIndex();
    TupProject::Mode mode = response->spaceMode();
    
    TupScene *scene = m_project->scene(scenePosition);
    
    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    QString strItems = "";
                    QList<QGraphicsItem *> items = frame->splitItemsGroup(position);
                    foreach (QGraphicsItem *item, items) {
                             if (frame->indexOf(item) != -1) {
                                 if (strItems.isEmpty())
                                     strItems += "("+ QString::number(frame->indexOf(item));
                                 else
                                     strItems += " , "+ QString::number(frame->indexOf(item));
                             } else {
                                 #ifdef K_DEBUG
                                     QString msg = "TupCommandExecutor::ungroupItems() - Error: Item wasn't found at frame!";
                                     #ifdef Q_OS_WIN32
                                         qDebug() << msg;
                                     #else
                                         tError() << msg;
                                     #endif
                                 #endif
                             }
                    }
                    strItems+= ")";
                    response->setArg(strItems);
                    emit responsed(response);

                    return true;
                }
            }

        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::ungroupItems() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

                if (frame) {
                    QString strItems = "";
                    QList<QGraphicsItem *> items = frame->splitItemsGroup(position);
                    foreach (QGraphicsItem *item, items) {
                             if (frame->indexOf(item) != -1) {
                                 if (strItems.isEmpty())
                                     strItems += "("+ QString::number(frame->indexOf(item));
                                 else
                                     strItems += " , "+ QString::number(frame->indexOf(item));
                             } else {
                                 #ifdef K_DEBUG
                                     QString msg = "TupCommandExecutor::ungroupItems() - Error: Item wasn't found at static/dynamic frame!";
                                     #ifdef Q_OS_WIN32
                                         qDebug() << msg;
                                     #else
                                         tError() << msg;
                                     #endif
                                 #endif
                             }
                    }
                    strItems+= ")";
                    response->setArg(strItems);
                    emit responsed(response);
                    return true;
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::ungroupItems() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::ungroupItems() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }

    return false;
}

static QGraphicsItem *convert(QGraphicsItem *item, int toType)
{
    /* SQA: Debugging tracers
    tFatal() << "TupPathItem::Type: " << TupPathItem::Type;
    tFatal() << "TupRectItem::Type: " <<  TupRectItem::Type;
    tFatal() << "TupEllipseItem::Type: " <<  TupEllipseItem::Type;
    tFatal() << "TupProxyItem::Type: " <<  TupProxyItem::Type;
    tFatal() << "TupLineItem::Type: " <<  TupLineItem::Type;
    */

    switch (toType) {
            case TupPathItem::Type: // Path
            {
                 TupPathItem *path = TupItemConverter::convertToPath(item);
                 return path;
            }
            break;
            case TupRectItem::Type: // Rect
            {
                 TupRectItem *rect = TupItemConverter::convertToRect(item);
                 return rect;
            }
            break;
            case TupEllipseItem::Type: // Ellipse
            {
                 TupEllipseItem *ellipse = TupItemConverter::convertToEllipse(item);
                 return ellipse;
            }
            break;
            case TupProxyItem::Type:
            {
                 return new TupProxyItem(item);
            }
            break;
            case TupLineItem::Type:
            {
                 return TupItemConverter::convertToLine(item);
            }
            break;
            default:
            {
            #ifdef K_DEBUG
                QString msg = "TupCommandExecutor::convert() - Error: Unknown item type -> " + QString::number(toType);
                #ifdef Q_OS_WIN32
                    qWarning() << msg;
                #else
                    tWarning() << msg;
                #endif
            #endif
            }
            break;
    }

    return 0;
}

bool TupCommandExecutor::convertItem(TupItemResponse *response)
{    
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::convertItem()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    int position = response->itemIndex();
    TupProject::Mode mode = response->spaceMode();
    int toType = response->arg().toInt();
    
    TupScene *scene = m_project->scene(scenePosition);

    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {

            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    QGraphicsItem *item = frame->item(position);
                    if (item) {
                        // tDebug("items") << item->type();
                    
                        if (toType == item->type()) 
                            return false;
                    
                        QGraphicsItem *itemConverted = convert(item, toType);
                        // tFatal() << "TupCommandExecutor::convertItem() - item new type: " << toType;
                    
                        if (itemConverted) {
                            // scene->removeItem(item); // FIXME?
                            // scene->addItem(itemConverted); // FIXME?
                            itemConverted->setZValue(item->zValue());
                            frame->replaceItem(position, itemConverted);
                        
                            response->setArg(QString::number(item->type()));
                            emit responsed(response);

                            return true;
                        }
                    }
                }
            }
        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::convertItem() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }
                if (frame) {
                    QGraphicsItem *item = frame->item(position);
                    if (item) {
                        // tDebug("items") << item->type();

                        if (toType == item->type())
                            return false;

                        QGraphicsItem * itemConverted = convert(item, toType);

                        if (itemConverted) {
                            itemConverted->setZValue(item->zValue());
                            frame->replaceItem(position, itemConverted);

                            response->setArg(QString::number(item->type()));
                            emit responsed(response);

                            return true;
                        }
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "TupCommandExecutor::convertItem() - Error: Invalid item index!";
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                        return false;
                    }
                } else {                    
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::convertItem() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::convertItem() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }

    return false;
}

bool TupCommandExecutor::transformItem(TupItemResponse *response)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::transformItem()]";
        #else
            T_FUNCINFOX("items");
        #endif
    #endif

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    int position = response->itemIndex();
    TupProject::Mode mode = response->spaceMode();
    TupLibraryObject::Type type = response->itemType();
    QString xml = response->arg().toString();

    TupScene *scene = m_project->scene(scenePosition);
    
    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    QGraphicsItem *item;
                    if (type == TupLibraryObject::Svg)
                        item = frame->svg(position);
                    else
                        item = frame->item(position);

                    if (item) {
                        QDomDocument orig;
                        orig.appendChild(TupSerializer::properties(item, orig));
                        QString current = orig.toString();

                        QDomDocument doc;
                        doc.setContent(xml);
                        TupSerializer::loadProperties(item, doc.documentElement());

                        response->setArg(current); 
                        emit responsed(response);
                    
                        return true;
                    } 
                }
            }
        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::transformItem() - Error: Invalid spaceMode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

                if (frame) {
                    QGraphicsItem *item;
                    if (type == TupLibraryObject::Svg)
                        item = frame->svg(position);
                    else
                        item = frame->item(position);

                    if (item) {
                        QDomDocument orig;
                        orig.appendChild(TupSerializer::properties(item, orig));
                        QString current = orig.toString();

                        QDomDocument doc;
                        doc.setContent(xml);
                        TupSerializer::loadProperties(item, doc.documentElement());

                        response->setArg(current);
                        emit responsed(response);

                        return true;
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "TupCommandExecutor::transformItem() - Error: Invalid item index!";
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                        return false;
                    }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::transformItem() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::transformItem() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }
    
    return false;
}

bool TupCommandExecutor::setPathItem(TupItemResponse *response)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::setPathItem()]";
            qDebug() << xml;
        #else
            T_FUNCINFOX("items");
            SHOW_VAR(xml);
        #endif
    #endif
    */
    
    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    int position = response->itemIndex();
    TupProject::Mode mode = response->spaceMode();
    QString xml = response->arg().toString();
    TupScene *scene = m_project->scene(scenePosition);
    
    if (scene) {
        if (mode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layer(layerPosition);
            if (layer) {
                TupFrame *frame = layer->frame(framePosition);
                if (frame) {
                    QGraphicsItem *item = frame->item(position);
                    if (item) {
                        if (qgraphicsitem_cast<QGraphicsPathItem *>(item)) {
                            QDomDocument orig;
                        
                            if (TupPathItem *path = qgraphicsitem_cast<TupPathItem *>(item))
                                orig.appendChild(path->toXml(orig));

                            QString current = orig.toString();
                            /*
                            #ifdef K_DEBUG
                                   SHOW_VAR(current);
                            #endif
                            */
                        
                            QDomDocument doc;
                            doc.setContent(xml);
                            TupItemFactory factory;
                            factory.loadItem(item, xml);
                            emit responsed(response);
                            response->setArg(current);

                            return true;
                       }
                    }
                }
            }

        } else {
            TupBackground *bg = scene->background();
            if (bg) {
                TupFrame *frame = 0;
                if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::setPathItem() - Error: Invalid mode!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

                if (frame) {
                    QGraphicsItem *item = frame->item(position);
                    if (item) {
                        if (qgraphicsitem_cast<QGraphicsPathItem *>(item)) {
                            QDomDocument orig;

                            if (TupPathItem *ktpath = qgraphicsitem_cast<TupPathItem *>(item))
                                orig.appendChild(ktpath->toXml(orig));

                            QString current = orig.toString();
                            QDomDocument doc;
                            doc.setContent(xml);
                            TupItemFactory factory;
                            factory.loadItem(item, xml);
                            emit responsed(response);
                            response->setArg(current);

                            return true;
                        }
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "TupCommandExecutor::setPathItem() - Invalid item index!";
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                        return false;
                    }
                } else {
                    #ifdef K_DEBUG
                        QString msg = "TupCommandExecutor::setPathItem() - Error: Invalid background frame!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return false;
                }

            } else {
                #ifdef K_DEBUG
                    QString msg = "TupCommandExecutor::setPathItem() - Error: Invalid background data structure!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return false;
            }
        }
    }

    return false;
}

bool TupCommandExecutor::setTween(TupItemResponse *response)
{    
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupCommandExecutor::setTween()]";
        #else
            T_FUNCINFO;
            SHOW_VAR(response);
        #endif
    #endif

    int scenePosition = response->sceneIndex();
    int layerPosition = response->layerIndex();
    int framePosition = response->frameIndex();
    TupLibraryObject::Type itemType = response->itemType();
    int position = response->itemIndex();
    QString xml = response->arg().toString();
    TupScene *scene = m_project->scene(scenePosition);
    
    if (scene) {
        TupLayer *layer = scene->layer(layerPosition);

        if (layer) {
            TupFrame *frame = layer->frame(framePosition);

            if (frame) {
                TupItemTweener *tween = new TupItemTweener();
                tween->fromXml(xml);

                if (itemType == TupLibraryObject::Item) {
                    TupGraphicObject *item = frame->graphic(position);
                    if (item) {
                        item->setTween(tween);
                        scene->addTweenObject(item);
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "TupCommandExecutor::setTween() - Error: Invalid graphic index -> " + QString::number(position);
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                        return false;
                    }
                } else {
                    TupSvgItem *svg = frame->svg(position); 
                    if (svg) {
                        svg->setTween(tween);
                        scene->addTweenObject(svg);
                    } else {
                        #ifdef K_DEBUG
                            QString msg = "TupCommandExecutor::setTween() - Error: Invalid svg index -> " + QString::number(position);
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                        return false;
                    }
                }

                emit responsed(response);

                return true;
            }
        }
    }
    
    return false;
}
