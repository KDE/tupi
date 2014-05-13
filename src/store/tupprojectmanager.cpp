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

#include "tupprojectmanager.h"
#include "tupproject.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupframe.h"

#include "tupprojectrequest.h"
#include "tupprojectcommand.h"
#include "tupcommandexecutor.h"
#include "tupprojectmanagerparams.h"
#include "tupabstractprojectmanagerhandler.h"
#include "tupprojectresponse.h"
#include "tuprequestbuilder.h"
#include "tuprequestparser.h"
#include "talgorithm.h"
#include "tdebug.h"

#include <QUndoStack>
#include <QDir>
#include <QFileInfo>

// This class handles the current animation project 

class TupProjectManager::Private
{
    public:
        Private() : handler(0), params(0)
        {
        }
        ~Private()
        {
           delete handler;
           delete undoStack;
           delete commandExecutor;
           delete params;
        }

    public:
        TupProject *project;
        bool isModified;
        int sceneIndex;
        int layerIndex;
        int frameIndex;
        TupAbstractProjectHandler *handler;
        QUndoStack *undoStack;
        TupCommandExecutor *commandExecutor;
        TupProjectManagerParams *params;
        QString copyFrame;
        bool isNetworked;
};

TupProjectManager::TupProjectManager(QObject *parent) : QObject(parent), k(new Private())
{
    #ifdef K_DEBUG
           TINIT;
    #endif
    
    k->isModified = false;
    k->handler = 0;

    k->project = new TupProject(this);
    k->undoStack = new QUndoStack(this);
    k->commandExecutor = new TupCommandExecutor(k->project);

    connect(k->commandExecutor, SIGNAL(responsed(TupProjectResponse*)), this, SLOT(emitResponse(TupProjectResponse *)));
    connect(k->project, SIGNAL(responsed(TupProjectResponse*)), this, SIGNAL(responsed(TupProjectResponse *)));
}

TupProjectManager::~TupProjectManager()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    delete k;
}

void TupProjectManager::setParams(TupProjectManagerParams *params)
{
    if (k->params) 
        delete k->params;

    k->params = params;

    k->handler->initialize(k->params);
}

TupProjectManagerParams *TupProjectManager::params() const
{
    return k->params;
}

void TupProjectManager::setHandler(TupAbstractProjectHandler *handler, bool isNetworked)
{
    if (k->handler) {
        disconnect(k->handler, SIGNAL(sendCommand(const TupProjectRequest *, bool)), 
                   this, SLOT(createCommand(const TupProjectRequest *, bool)));
        disconnect(k->handler, SIGNAL(sendLocalCommand(const TupProjectRequest *)), 
                   this, SLOT(handleLocalRequest(const TupProjectRequest *)));
        delete k->handler;
        k->handler = 0;
    }

    k->handler = handler;
    k->handler->setParent(this);
    k->handler->setProject(k->project);

    connect(k->handler, SIGNAL(sendCommand(const TupProjectRequest *, bool)), this, SLOT(createCommand(const TupProjectRequest *, bool)));
    connect(k->handler, SIGNAL(sendLocalCommand(const TupProjectRequest *)), this, SLOT(handleLocalRequest(const TupProjectRequest *)));

    k->isNetworked = isNetworked;
}

TupAbstractProjectHandler *TupProjectManager::handler() const
{
    return k->handler;
}

void TupProjectManager::setupNewProject()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!k->handler || !k->params) {
        #ifdef K_DEBUG
               tError() << "TupProjectManager::setupNewProject() - Error: No handler available or no params!";
        #endif
        return;
    }

    closeProject();

    k->project->setProjectName(k->params->projectName());
    k->project->setAuthor(k->params->author());
    k->project->setDescription(k->params->description());
    k->project->setBgColor(k->params->bgColor());
    k->project->setDimension(k->params->dimension());
    k->project->setFPS(k->params->fps());

    if (! k->handler->setupNewProject(k->params)) {
        #ifdef K_DEBUG
               tError() << "TupProjectManager::setupNewProject() - Error: Project params misconfiguration";
        #endif
        return;
    }

    if (!k->isNetworked) {
       
        QString projectPath = CACHE_DIR + k->params->projectName(); 
        cleanProjectPath(projectPath);

        k->project->setDataDir(projectPath);

        TupProjectRequest request = TupRequestBuilder::createSceneRequest(0, TupProjectRequest::Add, tr("Scene %1").arg(1));
        handleProjectRequest(&request);

        request = TupRequestBuilder::createLayerRequest(0, 0, TupProjectRequest::Add, tr("Layer %1").arg(1));
        handleProjectRequest(&request);

        request = TupRequestBuilder::createFrameRequest(0, 0, 0, TupProjectRequest::Add, tr("Frame %1").arg(1));
        handleProjectRequest(&request);
    }
}

void TupProjectManager::closeProject()
{
    if (!k->handler)
        return;

    if (k->project->isOpen()) {
        if (! k->handler->closeProject())
            return;

        k->project->clear();
    }

    k->project->setOpen(false);
    k->isModified = false;
    k->undoStack->clear();
}

bool TupProjectManager::saveProject(const QString &fileName)
{
    bool result = k->handler->saveProject(fileName, k->project);
    k->isModified = !result;

    return result;
}

bool TupProjectManager::loadProject(const QString &fileName)
{
    if (! k->handler) {
        #ifdef K_DEBUG
               tError() << "TupProjectManager::loadProject() - Fatal Error: No project handler available!";
        #endif
        return false;
    }

    bool ok = k->handler->loadProject(fileName, k->project);

    if (ok) {
        k->project->setOpen(true);
        k->isModified = false;
    } else {
        #ifdef K_DEBUG
               tError() << "TupProjectManager::loadProject() - Fatal Error: Can't load project -> " << fileName;
        #endif
    }

    return ok;
}

/**
 * Returns true if project is open
 */
bool TupProjectManager::isOpen() const
{
    return k->project->isOpen();
}

bool TupProjectManager::isModified() const
{
    return k->isModified;
}

void TupProjectManager::undoModified()
{
    k->isModified = false;
}

bool TupProjectManager::isValid() const
{
    if (!k->handler) 
        return false;

    return k->handler->isValid();
}

/*
void TupProjectManager::setupProjectDir()
{
    QString name = (k->project->projectName().isEmpty() ? TAlgorithm::randomString(6) : k->project->projectName());
    QString dataDir = CACHE_DIR + "/" + name;
    QDir projectDir = dataDir;

    if (!projectDir.exists()) {
        if (projectDir.mkpath(projectDir.absolutePath())) {
            QStringList dirs;
            dirs << "audio" << "video" << "images" << "svg";
            foreach (QString dir, dirs)
                     projectDir.mkdir(dir);
        }
    }
}
*/

/**
 * This function is called when some event is triggered by the project
 * It must be re-implemented if you want to deal with the event in another way, i.ex: send it through the net.
 * By default, it sends the event through the signal commandExecuted
 * @param event 
 */
void TupProjectManager::handleProjectRequest(const TupProjectRequest *request)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
           // SQA: Enable these lines only for hard/tough debugging
           // tWarning() << "Package: ";
           // tWarning() << request->xml();
    #endif

    // SQA: the handler must advise when to build the command
    
    if (k->handler) {
        k->handler->handleProjectRequest(request);
    } else {
        #ifdef K_DEBUG
               tError() << "TupProjectManager::handleProjectRequest() - Error: No handler available";
        #endif
    }
}

void TupProjectManager::handleLocalRequest(const TupProjectRequest *request)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TupRequestParser parser;

    if (parser.parse(request->xml())) {
        if (TupFrameResponse *response = static_cast<TupFrameResponse *>(parser.response())) {

            k->sceneIndex = response->sceneIndex();
            k->layerIndex = response->layerIndex();
            k->frameIndex = response->frameIndex();

            if (response->action() == TupProjectRequest::Copy) {
                TupScene *scene = k->project->scene(k->sceneIndex);
                if (scene) {
                    TupLayer *layer = scene->layer(k->layerIndex);
                    if (layer) {
                        TupFrame *frame = layer->frame(k->frameIndex);
                        if (frame) {
                            QDomDocument doc;
                            doc.appendChild(frame->toXml(doc));
                            k->copyFrame = doc.toString(0);
                            response->setArg(k->copyFrame);
                        }
                    }
                }
            } else if (response->action() == TupProjectRequest::Paste) {
                       response->setArg(k->copyFrame);

                       TupProjectRequest request = TupRequestBuilder::fromResponse(response);
                       handleProjectRequest(&request);
                       return;
            }
        }

        parser.response()->setExternal(request->isExternal());
        emit responsed(parser.response());
    }
}

/**
 * This function creates a command to execute an action, i.e. add a frame. 
 * The command has the information necessary to undo its effect.
 * Usually this command must be added in the commands stack.
 * The command created is not deleted by this class, this task depends on the user.
 * @param event 
 * @return 
 */
void TupProjectManager::createCommand(const TupProjectRequest *request, bool addToStack)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (request->isValid()) {
        TupProjectCommand *command = new TupProjectCommand(k->commandExecutor, request);

        if (addToStack)
            k->undoStack->push(command);
        else  
            command->redo();
    } else {
        #ifdef K_DEBUG
               tWarning() << "TupProjectManager::createCommand() - Invalid request";
        #endif
    }
}

TupProject *TupProjectManager::project() const
{
    return k->project;
}

QUndoStack *TupProjectManager::undoHistory() const
{
    return k->undoStack;
}

void TupProjectManager::emitResponse(TupProjectResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO << response->action();
    #endif

    if (response->action() != TupProjectRequest::Select) {
        k->isModified = true;
        // if (TupSceneResponse *sceneResponse = static_cast<TupSceneResponse *>(response)) {
        if (static_cast<TupSceneResponse *>(response)) {
            if (response->action() == TupProjectRequest::Remove)
                emit projectHasChanged(true);
            else
                emit projectHasChanged(false);
        } else {
            emit projectHasChanged(false);
        }
    }

    if (!k->handler) {
        // SQA: Check if this is the right way to handle this condition 
        emit responsed(response);
    } else if (k->isNetworked) {
               if (k->handler->commandExecuted(response))
                   emit responsed(response);
    } else { // Local request
        emit responsed(response);
    }
}

void TupProjectManager::setOpen(bool isOpen)
{
    k->project->setOpen(isOpen);
}

bool TupProjectManager::cleanProjectPath(QString &projectPath)
{
    bool result = true;
    QDir dir(projectPath);

    if (dir.exists(projectPath)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                QString path = info.absoluteFilePath();
                result = cleanProjectPath(path);
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(projectPath);
    }

    return result;
}

void TupProjectManager::updateProjectDimension(const QSize size)
{
    k->project->setDimension(size);     
}
