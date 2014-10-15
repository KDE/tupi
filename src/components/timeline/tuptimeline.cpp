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

#include "tuptimeline.h"

#define RETURN_IF_NOT_LIBRARY if (!k->library) return;

struct TupTimeLine::Private
{
    Private() : container(0), actionBar(0), selectedLayer(-1), library(0) {}
    
    TTabWidget *container;
    TupProjectActionBar *actionBar;
    int selectedLayer; 
    TupProject *project;
    TupLibrary *library;
};

TupTimeLine::TupTimeLine(TupProject *project, QWidget *parent) : TupModuleWidgetBase(parent, "TupTimeLine"), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine()]";
        #else
            TINIT;
        #endif
    #endif
    
    setWindowTitle(tr("Time Line"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/time_line.png"));

    k->project = project;
    k->library = k->project->library();
    
    k->actionBar = new TupProjectActionBar(QString("TimeLine"), TupProjectActionBar::InsertLayer |
                        TupProjectActionBar::RemoveLayer |
                        TupProjectActionBar::MoveLayerUp |
                        TupProjectActionBar::MoveLayerDown |
                        TupProjectActionBar::Separator |
                        TupProjectActionBar::InsertFrame |
                        TupProjectActionBar::RemoveFrame |
                        TupProjectActionBar::MoveFrameUp |
                        TupProjectActionBar::MoveFrameDown |
                        TupProjectActionBar::LockFrame);
   
    addChild(k->actionBar, Qt::AlignCenter);
    
    k->container = new TTabWidget(this);
    addChild(k->container);
    
    connect(k->actionBar, SIGNAL(actionSelected(int)), this, SLOT(requestCommand(int)));
    connect(k->container, SIGNAL(currentChanged(int)), this, SLOT(emitRequestChangeScene(int)));
}

TupTimeLine::~TupTimeLine()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~TupTimeLine()]";
        #else
            TEND;
        #endif
    #endif

    delete k;
}

TupFramesTable *TupTimeLine::framesTable(int sceneIndex)
{
    TupFramesTable *framesTable = qobject_cast<TupFramesTable *>(k->container->widget(sceneIndex));
    if (framesTable)
        return framesTable;
 
    return 0;
}

void TupTimeLine::insertScene(int sceneIndex, const QString &name)
{
    if (sceneIndex < 0 || sceneIndex > k->container->count())
        return;

    TupFramesTable *framesTable = new TupFramesTable(sceneIndex, k->container);
    framesTable->setItemSize(10, 20);

    connect(framesTable, SIGNAL(frameRequest(int, int, int, int, const QVariant&)), this, SLOT(requestFrameAction(int, int, int, int, const QVariant&)));
    connect(framesTable, SIGNAL(frameSelectionIsRequired(int, int)), this, SLOT(selectFrame(int, int)));
    connect(framesTable, SIGNAL(visibilityHasChanged(int, bool)), this, SLOT(requestLayerVisibilityAction(int, bool)));
    connect(framesTable, SIGNAL(layerNameHasChanged(int, const QString &)), this, SLOT(requestLayerRenameAction(int, const QString &))); 

    k->container->insertTab(sceneIndex, framesTable, name);
}

void TupTimeLine::removeScene(int sceneIndex)
{
    if (sceneIndex >= 0 && sceneIndex < k->container->count()) {
        QWidget *w = k->container->widget(sceneIndex);
        k->container->removeTab(sceneIndex);
        delete w;
    }
}

void TupTimeLine::closeAllScenes()
{
    while (k->container->currentWidget())
           delete k->container->currentWidget();
}

void TupTimeLine::sceneResponse(TupSceneResponse *response)
{
    Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::sceneResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    switch (response->action()) {
            case TupProjectRequest::Add:
            {
                 insertScene(response->sceneIndex(), response->arg().toString());
            }
            break;
            case TupProjectRequest::Remove:
            {
                 removeScene(response->sceneIndex());
            }
            break;
            case TupProjectRequest::Move:
            {
            
            }
            break;
            case TupProjectRequest::Lock:
            {
            
            }
            break;
            case TupProjectRequest::Rename:
            {
            
            }
            break;
            case TupProjectRequest::Select:
            {
                 k->container->setCurrentIndex(response->sceneIndex());
            }
            break;
            default:
                 tFatal() << "TupTimeLine::sceneResponse : Unknown action :/";
            break;
    }
}

void TupTimeLine::layerResponse(TupLayerResponse *response)
{
    Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::layerResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
    if (framesTable) {
        switch (response->action()) {
                case TupProjectRequest::Add:
                {
                     framesTable->insertLayer(response->layerIndex(), response->arg().toString());
                }
                break;
                case TupProjectRequest::Remove:
                {
                     framesTable->removeLayer(response->layerIndex());
                }
                break;
                case TupProjectRequest::Move:
                {
                     framesTable->moveLayer(response->layerIndex(), response->arg().toInt());
                }
                break;
                case TupProjectRequest::Lock:
                {
                     // SQA: Pending for implementation
                }
                break;
                case TupProjectRequest::Rename:
                {
                     framesTable->setLayerName(response->layerIndex(), response->arg().toString());
                }
                break;
                case TupProjectRequest::View:
                {
                     framesTable->setLayerVisibility(response->layerIndex(), response->arg().toBool());
                }
                break;
        }
    }
}

void TupTimeLine::frameResponse(TupFrameResponse *response)
{
    Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::frameResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
    if (framesTable) {
        switch (response->action()) {
                case TupProjectRequest::Add:
                {
                     framesTable->insertFrame(response->layerIndex(), response->arg().toString());
                }
                break;
                case TupProjectRequest::Remove:
                {
                     framesTable->removeFrame(response->layerIndex(), response->frameIndex());
                }
                break;
                case TupProjectRequest::Move:
                {
                }
                break;
                case TupProjectRequest::Lock:
                {
                     framesTable->lockFrame(response->layerIndex(), response->frameIndex(), response->arg().toBool());
                }
                break;
                case TupProjectRequest::Rename:
                {
                }
                break;
                case TupProjectRequest::Select:
                {
                     int layerIndex = response->layerIndex();
                     k->selectedLayer = layerIndex;

                     framesTable->blockSignals(true);
                     framesTable->setCurrentCell(layerIndex, response->frameIndex());
                     framesTable->updateLayerHeader(layerIndex);
                     framesTable->blockSignals(false);
                }
                break;
        }
    }
}

void TupTimeLine::libraryResponse(TupLibraryResponse *response)
{
    Q_UNUSED(response);

    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::libraryResponse()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (response->action() == TupProjectRequest::InsertSymbolIntoFrame) {
        switch (response->symbolType()) {
                case TupLibraryObject::Sound:
                {
                     TupFramesTable *framesTable = this->framesTable(response->sceneIndex());
                     if (framesTable) {
                         framesTable->insertSoundLayer(response->layerIndex() + 1, response->arg().toString());
                         framesTable->insertFrame(response->layerIndex() + 1, "");
                     }
                }
                break;
                default:
                    // Do nothing
                break;
        };
    }
}

void TupTimeLine::requestCommand(int action)
{
    int scenePos = k->container->currentIndex();

    if (scenePos < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Scene index is invalid -> " + QString::number(scenePos);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    int layerPos = framesTable(scenePos)->currentLayer();

    if (layerPos < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Layer index is invalid -> " + QString::number(layerPos);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    int framePos = framesTable(scenePos)->lastFrameByLayer(layerPos) + 1;

    if (framePos < 0) {
        #ifdef K_DEBUG
            QString msg = "TupTimeLine::requestCommand() - Fatal Error: Frame index is invalid -> " + QString::number(framePos);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal("timeline") << msg;
            #endif
        #endif

        return;
    }

    if (TupProjectActionBar::FrameActions & action) {
        if (!requestFrameAction(action, framePos, layerPos, scenePos)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Frame action has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }

    if (TupProjectActionBar::LayerActions & action) {
        if (!requestLayerAction(action, layerPos, scenePos)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Layer action has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }

    if (TupProjectActionBar::SceneActions & action) {
        if (!requestSceneAction(action, scenePos)) {
            #ifdef K_DEBUG
                QString msg = "TupTimeLine::requestCommand() - Fatal Error: Scene action has failed!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal("timeline") << msg;
                #endif
            #endif
        }
        return;
    }
}

bool TupTimeLine::requestFrameAction(int action, int framePos, int layerPos, int scenePos, const QVariant &arg)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::requestFrameAction()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    TupProjectRequest event;

    switch (action) {
            case TupProjectActionBar::InsertFrame:
            {
                 int layersTotal = framesTable(scenePos)->layersTotal();
                 int usedFrames = framesTable(scenePos)->lastFrameByLayer(layerPos);

                 if (layersTotal == 1) {
                     for (int frame = usedFrames; frame < framePos; frame++) {
                          event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, frame + 1,
                                                     TupProjectRequest::Add, tr("Frame %1").arg(frame + 2));
                          emit requestTriggered(&event);
                     }

                 } else {
                     usedFrames++;

                     for (int layer=0; layer < layersTotal; layer++) {
                          for (int frame = usedFrames; frame <= framePos; frame++) {
                               event = TupRequestBuilder::createFrameRequest(scenePos, layer, frame,
                                                          TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                               emit requestTriggered(&event);
                          }
                     }
                 }
            
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveFrame:
            {
                 event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos - 1,
                                            TupProjectRequest::Remove, arg);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameUp:
            {
                 event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                            TupProjectRequest::Move, framePos - 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveFrameDown:
            {
                 event = TupRequestBuilder::createFrameRequest(scenePos, layerPos, framePos,
                                            TupProjectRequest::Move, framePos + 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            default:
                 // Do nothing
            break;
    }
    
    return false;
}

bool TupTimeLine::requestLayerAction(int action, int layerPos, int scenePos, const QVariant &arg)
{
    TupProjectRequest event;

    switch (action) {
            case TupProjectActionBar::InsertLayer:
            {
                 int layerIndex = framesTable(scenePos)->layersTotal();
                 event = TupRequestBuilder::createLayerRequest(scenePos, layerIndex,
                                            TupProjectRequest::Add, tr("Layer %1").arg(layerIndex + 1));
                 emit requestTriggered(&event);

                 if (layerIndex == 0) {
                     event = TupRequestBuilder::createFrameRequest(scenePos, layerIndex, 0,
                                                TupProjectRequest::Add, tr("Frame %1").arg(1));
                     emit requestTriggered(&event);
                 } else {
                     int total = framesTable(scenePos)->lastFrameByLayer(layerIndex - 1);
                     for (int j=0; j <= total; j++) {
                          event = TupRequestBuilder::createFrameRequest(scenePos, layerIndex, j,
                                                                       TupProjectRequest::Add, tr("Frame %1").arg(j + 1));
                          emit requestTriggered(&event);
                     }
                 }

                 return true;
            }
            break;
            case TupProjectActionBar::RemoveLayer:
            {
                 event = TupRequestBuilder::createLayerRequest(scenePos, layerPos, TupProjectRequest::Remove, arg);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveLayerUp:
            {
                 event = TupRequestBuilder::createLayerRequest(scenePos, layerPos, 
                                            TupProjectRequest::Move, layerPos - 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveLayerDown:
            {
                 event = TupRequestBuilder::createLayerRequest(scenePos, layerPos, 
                                            TupProjectRequest::Move, layerPos + 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
    }
    
    return false;
}

bool TupTimeLine::requestSceneAction(int action, int scenePos, const QVariant &arg)
{
    TupProjectRequest event;

    switch (action) {
            case TupProjectActionBar::InsertScene:
            {
                 event = TupRequestBuilder::createSceneRequest(scenePos + 1, TupProjectRequest::Add, tr("Scene %1").arg(scenePos + 2));
                 emit requestTriggered(&event);
            
                 return true;
            }
            break;
            case TupProjectActionBar::RemoveScene:
            {
                 event = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Remove, arg);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneUp:
            {
                 event = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Move, scenePos + 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
            case TupProjectActionBar::MoveSceneDown:
            {
                 event = TupRequestBuilder::createSceneRequest(scenePos, TupProjectRequest::Move, scenePos - 1);
                 emit requestTriggered(&event);

                 return true;
            }
            break;
    }
    
    return false;
}

void TupTimeLine::requestLayerVisibilityAction(int layer, bool isVisible)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::requestLayerVisibilityAction()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    int scenePos = k->container->currentIndex();

    TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layer, TupProjectRequest::View, isVisible);
    emit requestTriggered(&event);
}

void TupTimeLine::requestLayerRenameAction(int layer, const QString &name)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLine::requestLayerRenameAction()]";
            qDebug() << "name: " << name;
        #else
            T_FUNCINFO << name;
        #endif
    #endif
    */

    int scenePos = k->container->currentIndex();
    
    TupProjectRequest event = TupRequestBuilder::createLayerRequest(scenePos, layer, TupProjectRequest::Rename, name);
    emit requestTriggered(&event);
}

void TupTimeLine::selectFrame(int indexLayer, int indexFrame)
{
    int scenePos = k->container->currentIndex();
    TupScene *scene = k->project->scene(scenePos);
    if (scene) {
        int totalFrames = scene->framesTotal();
        tError() << "TupTimeLine::selectFrame() - Total frames: " << totalFrames;
        if (indexFrame < totalFrames) {
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, indexLayer,
                                                           indexFrame, TupProjectRequest::Select, "1");
            emit requestTriggered(&request);
        } else {
            int layersTotal = scene->layersTotal();
            for (int layer=0; layer < layersTotal; layer++) {
                 for (int frame = totalFrames; frame <= indexFrame; frame++) {
                      TupProjectRequest event = TupRequestBuilder::createFrameRequest(scenePos, layer, frame,
                                                TupProjectRequest::Add, tr("Frame %1").arg(frame + 1));
                      emit requestTriggered(&event);
                 }
            }

            TupProjectRequest request = TupRequestBuilder::createFrameRequest(scenePos, indexLayer,
                                                           indexFrame, TupProjectRequest::Select, "1");
            emit requestTriggered(&request);
        }
    }
}

void TupTimeLine::emitRequestChangeScene(int sceneIndex)
{
    if (k->container->count() > 1) {
        // tFatal() << "TupTimeLine::emitRequestChangeScene - Just tracing!";
        TupProjectRequest request = TupRequestBuilder::createSceneRequest(sceneIndex, TupProjectRequest::Select);
        emit localRequestTriggered(&request);
    }
}

void TupTimeLine::emitRequestChangeFrame(int sceneIndex, int layerIndex, int frameIndex)
{
    // tFatal() << "TupTimeLine::emitRequestChangeFrame - Just tracing!";
    TupProjectRequest event = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, frameIndex,
                             TupProjectRequest::Select, "1");
    emit requestTriggered(&event);
}
