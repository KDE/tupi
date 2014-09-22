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

#include "papagayotool.h"
#include "configurator.h"
#include "target.h"
#include "taction.h"
#include "tupinputdeviceinformation.h"
#include "tupbrushmanager.h"
#include "tupgraphicsscene.h"
#include "tuprequestbuilder.h"
#include "tupprojectrequest.h"
#include "tupscene.h"

struct PapagayoTool::Private
{
    QMap<QString, TAction *> actions;
    Configurator *configurator;

    TupGraphicsScene *scene;

    TupLipSync *currentLipSync;
    int initFrame;
    int initLayer;
    int initScene;

    QPointF origin;
    Target *target;

    TupToolPlugin::Mode mode;

    int baseZValue;

    QGraphicsItem *mouth;
    QPointF mouthOffset;
    QString currentMouth;
    int currentMouthIndex;
};

PapagayoTool::PapagayoTool() : TupToolPlugin(), k(new Private)
{
    setupActions();

    k->configurator = 0;
    k->initFrame = 0;
    k->target = 0;
}

PapagayoTool::~PapagayoTool()
{
    delete k;
}

/* This method initializes the plugin */

void PapagayoTool::init(TupGraphicsScene *scene)
{
    k->scene = scene;
    k->mode = TupToolPlugin::View;
    k->baseZValue = 20000 + (scene->scene()->layersTotal() * 10000);
    k->initFrame = k->scene->currentFrameIndex();
    k->initScene = k->scene->currentSceneIndex();

    removeTarget();

    k->configurator->resetUI();

    QList<QString> lipSyncList = k->scene->scene()->getLipSyncNames();
    if (lipSyncList.size() > 0) {
        k->configurator->loadLipSyncList(lipSyncList);
        QString lipSyncName = lipSyncList.at(0);
        TupScene *currentScene = k->scene->scene();
        k->currentLipSync = currentScene->getLipSync(lipSyncName);
        if (k->currentLipSync)
            k->configurator->setCurrentLipSync(k->currentLipSync);
    }
}

/*
void PapagayoTool::updateStartPoint(int index)
{
     if (k->initFrame != index && index >= 0)
         k->initFrame = index;
}
*/

/* This method returns the plugin name */

QStringList PapagayoTool::keys() const
{
    return QStringList() << tr("Papagayo Lip-sync");
}

/* This method makes an action when the mouse is pressed on the workspace 
 * depending on the active mode: Selecting an object or Creating a path  
*/

void PapagayoTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[PapagayoTool::press()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method is executed while the mouse is pressed and on movement */

void PapagayoTool::move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method finishes the action started on the press method depending
 * on the active mode: Selecting an object or Creating a path
*/

void PapagayoTool::release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[PapagayoTool::release()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(input);
    Q_UNUSED(brushManager);
    Q_UNUSED(scene);
}

/* This method returns the list of actions defined in this plugin */

QMap<QString, TAction *> PapagayoTool::actions() const
{
    return k->actions;
}

/* This method returns the list of actions defined in this plugin */

int PapagayoTool::toolType() const
{
    return TupToolInterface::LipSync;
}

/* This method returns the tool panel associated to this plugin */

QWidget *PapagayoTool::configurator()
{
    if (!k->configurator) {
        k->mode = TupToolPlugin::View;

        k->configurator = new Configurator;
        connect(k->configurator, SIGNAL(importLipSync()), this, SIGNAL(importLipSync()));
        connect(k->configurator, SIGNAL(removeCurrentLipSync(const QString &)), this, SLOT(removeCurrentLipSync(const QString &)));
        connect(k->configurator, SIGNAL(selectMouth(const QString &, int)), this, SLOT(addTarget(const QString &, int)));
        connect(k->configurator, SIGNAL(updateLipSyncSelection(const QString &)), this, SLOT(setCurrentLipSync(const QString &)));
        connect(k->configurator, SIGNAL(closeLipSyncProperties()), this, SLOT(resetCanvas()));
        connect(k->configurator, SIGNAL(initFrameHasChanged(int)), this, SLOT(updateInitFrame(int)));
    } 

    return k->configurator;
}

void PapagayoTool::resetCanvas()
{
    k->mode = TupToolPlugin::View;
    removeTarget();
}

/* This method is called when there's a change on/of scene */
void PapagayoTool::aboutToChangeScene(TupGraphicsScene *)
{
}

/* This method is called when this plugin is off */

void PapagayoTool::aboutToChangeTool()
{
}

/* This method defines the actions contained in this plugin */

void PapagayoTool::setupActions()
{
    TAction *translater = new TAction(QPixmap(kAppProp->themeDir() + "icons" + QDir::separator() + "papagayo.png"), 
                                      tr("Papagayo Lip-sync"), this);
    translater->setShortcut(QKeySequence(tr("Ctrl+Shift+P")));

    k->actions.insert(tr("Papagayo Lip-sync"), translater);
}

/* This method saves the settings of this plugin */

void PapagayoTool::saveConfig()
{
}

/* This method updates the workspace when the plugin changes the scene */

void PapagayoTool::updateScene(TupGraphicsScene *scene)
{ 
    Q_UNUSED(scene); 
}

void PapagayoTool::setCurrentLipSync(const QString &name)
{
    TupScene *scene = k->scene->scene();
    int initFrame = k->currentLipSync->initFrame();

    if (initFrame != k->scene->currentFrameIndex()) {
        int initLayer = scene->getLipSyncLayerIndex(name);

        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->initScene, initLayer, initFrame,
                                                                          TupProjectRequest::Select, "1");
        emit requested(&request);
    }

    if (name.compare(k->currentLipSync->name()) != 0) {
        k->currentLipSync = scene->getLipSync(name);
        if (k->currentLipSync)
            k->configurator->setCurrentLipSync(k->currentLipSync);
    }
}

void PapagayoTool::removeCurrentLipSync(const QString &name)
{
    QGraphicsView * view = k->scene->views().first();
    foreach (QGraphicsItem *item, view->scene()->items()) {
             QString tip = item->toolTip();
             if (tip.length() > 0) {
                 if (tip.startsWith(tr("lipsync:") + name))
                     k->scene->removeItem(item);
             }
    }

    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->initScene, k->initLayer, TupProjectRequest::RemoveLipSync, name);
    emit requested(&request);
}

void PapagayoTool::updateOriginPoint(const QPointF &point)
{
    k->origin = point - k->mouthOffset;
    k->mouth->setPos(k->origin);
    k->configurator->setMouthPos(k->origin);
}

void PapagayoTool::addTarget(const QString &id, int index)
{
    k->mode = TupToolPlugin::Edit;

    k->currentMouth = id;
    k->currentMouthIndex = index;
    addTarget();
}

void PapagayoTool::addTarget()
{
    TupScene *scene = k->scene->scene();
    int initLayer = scene->getLipSyncLayerIndex(k->currentLipSync->name());
    int initFrame = k->currentLipSync->initFrame();

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->initScene, initLayer, initFrame,
                                                                          TupProjectRequest::Select, "1");
    emit requested(&request);

    removeTarget();
    setTargetEnvironment();
}

void PapagayoTool::setTargetEnvironment()
{
    QGraphicsView *view = k->scene->views().at(0);
    foreach (QGraphicsItem *item, view->scene()->items()) {
             QString tip = item->toolTip();
             if (tip.length() > 0) {
                 if (tip.compare(k->currentMouth) == 0) {
                     k->mouthOffset = item->boundingRect().center();
                     k->origin = item->pos() + k->mouthOffset;
                     k->mouth = item;
                 }
             }
    }

    k->target = new Target(k->origin, k->baseZValue);
    connect(k->target, SIGNAL(positionUpdated(const QPointF &)), this, SLOT(updateOriginPoint(const QPointF &)));
    k->scene->addItem(k->target);
}

void PapagayoTool::removeTarget()
{
    if (k->target) {
        k->scene->removeItem(k->target);
        k->target = 0;
    }
}

void PapagayoTool::sceneResponse(const TupSceneResponse *event)
{
    if ((event->action() == TupProjectRequest::Remove || event->action() == TupProjectRequest::Reset)
        && (k->scene->currentSceneIndex() == event->sceneIndex()))
        init(k->scene);

    if (event->action() == TupProjectRequest::Select)
        init(k->scene);
}

void PapagayoTool::layerResponse(const TupLayerResponse *event)
{
    if (event->action() == TupProjectRequest::UpdateLipSync) {
        setCurrentLipSync(k->currentLipSync->name());
        k->configurator->updateInterfaceRecords();

        addTarget();

        TupScene *scene = k->scene->scene();
        int sceneFrames = scene->framesTotal();
        int lipSyncFrames = k->currentLipSync->initFrame() + k->currentLipSync->framesTotal();
        k->initFrame = k->scene->currentFrameIndex();
        k->initLayer = scene->getLipSyncLayerIndex(k->currentLipSync->name());

        if (lipSyncFrames > sceneFrames) {
            int layersTotal = scene->layersTotal();
            for (int i = sceneFrames; i < lipSyncFrames; i++) {
                 for (int j = 0; j < layersTotal; j++) {
                      TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->initScene, j, i, TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
                      emit requested(&request);
                 }
            }
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->initScene, k->initLayer, k->initFrame, TupProjectRequest::Select, "1");
            emit requested(&request);
        }
    }
}

void PapagayoTool::frameResponse(const TupFrameResponse *event)
{
    Q_UNUSED(event);

    if (event->action() == TupProjectRequest::Select) {
        if (k->mode == TupToolPlugin::Edit) {
            int frameIndex = event->frameIndex();
            int lastFrame = k->currentLipSync->initFrame() + k->currentLipSync->framesTotal() - 1;
            if (frameIndex >= k->currentLipSync->initFrame() && frameIndex <= lastFrame)
                setTargetEnvironment();
        }
    }
}

void PapagayoTool::addNewItem(const QString &name)
{
    k->configurator->addLipSync(name);
}

void PapagayoTool::updateWorkSpaceContext()
{
    if (k->mode == TupToolPlugin::Edit)
        k->configurator->closePanels();
}

void PapagayoTool::updateInitFrame(int index)
{
    removeTarget();

    k->currentLipSync->setInitFrame(index);

    QDomDocument document;
    QDomElement root = k->currentLipSync->toXml(document);
    QString xml;
    {
      QTextStream ts(&xml);
      ts << root;
    }

    TupScene *scene = k->scene->scene();
    k->initLayer = scene->getLipSyncLayerIndex(k->currentLipSync->name());

    TupProjectRequest request = TupRequestBuilder::createLayerRequest(k->initScene, k->initLayer, TupProjectRequest::UpdateLipSync, xml);
    emit requested(&request);
}

