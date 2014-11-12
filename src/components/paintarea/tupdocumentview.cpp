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

#include "tupdocumentview.h"
#include "tupapplication.h"
#include "tconfig.h"
#include "tuprequestbuilder.h"
#include "tuppaintareaproperties.h"
#include "tuppluginmanager.h"
#include "tupexportinterface.h"
#include "tuppaintarea.h"
#include "tupprojectresponse.h"
#include "tuppaintareaevent.h"
#include "tuppaintareacommand.h"
#include "tupgraphicsscene.h"
#include "tupbrushmanager.h"
#include "tupproject.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tuppaintareastatus.h"
#include "tupcanvas.h"
#include "polylinetool.h"
#include "tupimagedialog.h"
#include "tupstoryboarddialog.h"
#include "tupruler.h"
#include "tupcamerainterface.h"
#include "tupbasiccamerainterface.h"
#include "tupcameradialog.h"
#include "tuplibrary.h"
#include "tuppapagayoimporter.h"
#include "tuppapagayodialog.h"

/**
 * This class defines all the environment for the Ilustration interface.
 * Here is where all the tools for Ilustration are defined.
 * @author David Cuadrado
*/

struct TupDocumentView::Private
{
    QSize wsDimension;

    QMenu *shapesMenu;
    QMenu *motionMenu;

    QMenu *filterMenu;
    QMenu *toolsMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *orderMenu;

    QToolBar *barGrid;
    QToolBar *toolbar;
    QToolBar *propertiesBar;

    QDoubleSpinBox *onionFactorSpin;
    QSpinBox *prevOnionSkinSpin;
    QSpinBox *nextOnionSkinSpin;

    QComboBox *dirCombo;
    QSpinBox *shiftSpin;

    bool onionEnabled;
    int prevOnionValue;
    int nextOnionValue;
    double opacityFactor;
    int viewAngle;
    int autoSaveTime;
    bool fullScreenOn;
    bool isNetworked;
    QStringList onLineUsers;

    TupPaintArea *paintArea;
    TupCanvas *fullScreen;

    TupRuler *verticalRuler;
    TupRuler *horizontalRuler;

    TActionManager *actionManager;
    TupConfigurationArea *configurationArea;
    TupToolPlugin *currentTool;
    TupPaintAreaStatus *status;
    QComboBox *spaceMode;
    bool dynamicFlag;
    QSize cameraSize;
    int photoCounter;

    TupProject *project;
    QTimer *timer;

    TupExportInterface *imagePlugin;

    qreal nodesScaleFactor;
    qreal cacheScaleFactor;

    TAction *pencilAction;
    TAction *inkAction;
    TAction *polyLineAction;
    TAction *selectionAction;
    TAction *nodesAction;
    TAction *borderFillAction;
    TAction *fillAction;
    TAction *handAction;
    TAction *papagayoAction;
};

TupDocumentView::TupDocumentView(TupProject *project, QWidget *parent, bool isNetworked, const QStringList &users) : QMainWindow(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupDocumentView()]";
        #else
            TINIT;
        #endif
    #endif

    setWindowIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "animation_mode.png"));

    k->project = project;
    k->currentTool = 0;
    k->onionEnabled = true;
    k->fullScreenOn = false;
    k->viewAngle = 0;
    k->isNetworked = isNetworked;
    k->onLineUsers = users;
    k->dynamicFlag = false;
    k->photoCounter = 1;
    k->nodesScaleFactor = 1;

    k->actionManager = new TActionManager(this);

    QFrame *frame = new QFrame(this, Qt::FramelessWindowHint);
    QGridLayout *layout = new QGridLayout(frame);

    k->paintArea = new TupPaintArea(project, frame);

    TCONFIG->beginGroup("OnionParameters");
    k->opacityFactor = TCONFIG->value("OnionFactor", -1).toDouble();
    if (k->opacityFactor < 0)
        k->opacityFactor = 0.5;
    k->paintArea->setOnionFactor(k->opacityFactor);

    setCentralWidget(frame);

    layout->addWidget(k->paintArea, 1, 1);

    k->horizontalRuler = new TupRuler(Qt::Horizontal, this);
    k->verticalRuler = new TupRuler(Qt::Vertical, this);
    layout->addWidget(k->horizontalRuler, 0, 1);
    layout->addWidget(k->verticalRuler, 1, 0);

    connect(k->paintArea, SIGNAL(scaled(qreal)), this, SLOT(updateZoomVars(qreal)));
    connect(k->paintArea, SIGNAL(rotated(int)), this, SLOT(updateRotationVars(int)));
    connect(k->paintArea, SIGNAL(updateStatusBgColor(QColor)), this, SLOT(updateStatusBgColor(QColor)));

    Tupi::RenderType renderType = Tupi::RenderType(TCONFIG->value("RenderType").toInt()); 

    switch (renderType) {
            case Tupi::OpenGL:
                 k->paintArea->setUseOpenGL(true);
                 break;

            case Tupi::Native:
                 k->paintArea->setUseOpenGL(false);
                 break;

            default:
                 #ifdef K_DEBUG
                     QString msg = "TupDocumentView() - Unsopported render, switching to native!";
                     #ifdef Q_OS_WIN32
                         qWarning() << msg;
                     #else
                         tWarning() << msg;
                     #endif
                 #endif
                 k->paintArea->setUseOpenGL(false);
            break;
    }
    
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), this, SLOT(showPos(const QPointF &)));

    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), k->verticalRuler, SLOT(movePointers(const QPointF&)));
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), k->horizontalRuler, SLOT(movePointers(const QPointF&)));

    connect(k->paintArea, SIGNAL(changedZero(const QPointF&)), this, SLOT(changeRulerOrigin(const QPointF&)));

    connect(k->paintArea, SIGNAL(requestTriggered(const TupProjectRequest *)), this, SIGNAL(requestTriggered(const TupProjectRequest *)));
    connect(k->paintArea, SIGNAL(localRequestTriggered(const TupProjectRequest *)), this, SIGNAL(localRequestTriggered(const TupProjectRequest *)));

    setupDrawActions();

    createLateralToolBar(); 
    createToolBar();
    
    k->status = new TupPaintAreaStatus(this);
    setStatusBar(k->status);
    connect(k->status, SIGNAL(colorRequested()), this, SIGNAL(expandColorPanel()));
    connect(k->status, SIGNAL(colorUpdated(const QColor)), this, SLOT(updateBgColor(const QColor)));
    connect(k->status, SIGNAL(newFramePointer(int)), k->paintArea, SLOT(goToFrame(int)));
    connect(k->paintArea, SIGNAL(frameChanged(int)), k->status, SLOT(updateFrameIndex(int)));

    // SQA: Verify if this code is doing something / SLOT setBrush() doesn't exist
    // connect(k->paintArea->brushManager(), SIGNAL(brushChanged(const QBrush&)), k->status, 
    //         SLOT(setBrush(const QBrush &)));

    connect(k->paintArea->brushManager(), SIGNAL(penChanged(const QPen&)), k->status, SLOT(setPen(const QPen &)));

    // SQA: Find out why this timer instruction is required?
    QTimer::singleShot(500, this, SLOT(loadPlugins()));

    // SQA: Temporarily disabled  
    // if (!k->isNetworked)
    //     saveTimer();
}

TupDocumentView::~TupDocumentView()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~TupDocumentView()]";
        #else
            TEND;
        #endif
    #endif

    TCONFIG->beginGroup("General");
    TCONFIG->setValue("AutoSave", k->autoSaveTime);

    if (k->currentTool)
        k->currentTool->saveConfig();

    delete k->configurationArea;
    delete k;
}

void TupDocumentView::setWorkSpaceSize(int width, int height)
{
    k->wsDimension = QSize(width, height);
}

void TupDocumentView::setAntialiasing(bool useIt)
{
    k->paintArea->setAntialiasing(useIt);
}

void TupDocumentView::setOpenGL(bool useIt)
{
    k->paintArea->setUseOpenGL(useIt);
}

void TupDocumentView::drawGrid()
{
    k->paintArea->drawGrid(!k->paintArea->gridFlag());
}

void TupDocumentView::drawActionSafeArea()
{
    k->paintArea->drawActionSafeArea(!k->paintArea->actionSafeAreaFlag());
}

QPainter::RenderHints TupDocumentView::renderHints() const
{
    return k->paintArea->renderHints();
}

void TupDocumentView::updateRotationAngleFromRulers(int angle)
{
    k->viewAngle = angle;

    TupRuler::Transformation flag = TupRuler::None;
    if (angle != 0 && angle != 90 && angle != 180  && angle != 270)
        flag = TupRuler::Rotation;
    else
        flag = TupRuler::None;

    k->verticalRuler->updateCurrentTransformation(flag);
    k->horizontalRuler->updateCurrentTransformation(flag);
}

void TupDocumentView::setRotationAngle(int angle)
{
    updateRotationAngleFromRulers(angle);

    k->paintArea->setRotationAngle(angle);
}

void TupDocumentView::updateRotationVars(int angle)
{
    updateRotationAngleFromRulers(angle);

    k->status->updateRotationAngle(angle);
}

void TupDocumentView::setZoomFactor(qreal factor)
{
    k->paintArea->setZoom(factor);
    k->verticalRuler->setRulerZoom(factor);
    k->horizontalRuler->setRulerZoom(factor);

    updateNodesScale(factor);
}

void TupDocumentView::updateZoomVars(qreal factor)
{
    k->status->updateZoomFactor(factor);
    k->verticalRuler->setRulerZoom(factor);
    k->horizontalRuler->setRulerZoom(factor);

    updateNodesScale(factor);
}

void TupDocumentView::updateNodesScale(qreal factor)
{
    if (k->currentTool) {
        k->nodesScaleFactor *= factor;
        QString toolName = k->currentTool->name();
        if (toolName.compare(tr("Object Selection")) == 0 || toolName.compare(tr("Nodes Selection")) == 0 || toolName.compare(tr("PolyLine")) == 0)
            k->currentTool->resizeNodes(1 / k->nodesScaleFactor);
    }
}

void TupDocumentView::setZoomPercent(const QString &percent)
{
    k->status->setZoomPercent(percent);
}

void TupDocumentView::showPos(const QPointF &point)
{
    QString message =  "X: " +  QString::number(point.x()) + " Y: " + QString::number(point.y());
    k->status->updatePosition(message);
}

void TupDocumentView::setupDrawActions()
{
    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "copy.png"), tr("Copy"), QKeySequence(tr("Ctrl+C")), 
                k->paintArea, SLOT(copyItems()), k->actionManager, "copy");

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "paste.png"), tr("Paste"), QKeySequence(tr("Ctrl+V")),
                k->paintArea, SLOT(pasteItems()), k->actionManager, "paste");

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "cut.png"), tr("Cut"), QKeySequence(tr("Ctrl+X")),
                k->paintArea, SLOT(cutItems()),k->actionManager, "cut");

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "delete.png"), tr("Delete"), QKeySequence(Qt::Key_Delete), 
                k->paintArea, SLOT(deleteItems()), k->actionManager, "delete");
    
    TAction *group = new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "group.png"), tr("&Group"), QKeySequence(tr("Ctrl+G")), 
                k->paintArea, SLOT(groupItems()), k->actionManager, "group");
    group->setDisabled(true);

    TAction *ungroup = new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "ungroup.png"), tr("&Ungroup"), 
                                    QKeySequence(tr("Ctrl+Shift+G")) , k->paintArea, SLOT(ungroupItems()), 
                                    k->actionManager, "ungroup");
    ungroup->setDisabled(true);

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "layer.png"), tr("Onion Skin"), QKeySequence(tr("Ctrl+Shift+O")), 
                this, SLOT(enableOnionFeature()), k->actionManager, "onion");

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "onion.png"), tr("Onion Skin Factor"), QKeySequence(tr("Ctrl+Shift+S")), 
                this, SLOT(setDefaultOnionFactor()), k->actionManager, "onionfactor");

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "export_frame.png"), tr("Export Current Frame As Image"), QKeySequence(tr("@")),
                this, SLOT(exportImage()), k->actionManager, "export_image");

    TCONFIG->beginGroup("Network");
    QString server = TCONFIG->value("Server").toString();

    if (k->isNetworked && server.compare("tupitu.be") == 0) {
        new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "import_project.png"), tr("Export Current Frame To Gallery"), QKeySequence(tr("@")),
                    this, SLOT(postImage()), k->actionManager, "post_image");
    }

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "storyboard.png"), tr("Storyboard Settings"), QKeySequence(tr("Ctrl+Shift+S")),
                this, SLOT(storyboardSettings()), k->actionManager, "storyboard");

    #ifdef Q_OS_WIN32
        if (QSysInfo::windowsVersion() != QSysInfo::WV_XP) {
            new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png"), tr("Camera"), QKeySequence(tr("Ctrl+Shift+C")),
                        this, SLOT(cameraInterface()), k->actionManager, "camera");
        }
    #else
        new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "camera.png"), tr("Camera"), QKeySequence(tr("Ctrl+Shift+C")),
                    this, SLOT(cameraInterface()), k->actionManager, "camera");
    #endif

    new TAction(QPixmap(THEME_DIR + "icons" + QDir::separator() + "papagayo.png"), tr("Papagayo Lip-sync Files"), QKeySequence(tr("Ctrl+Shift+P")),
                this, SLOT(papagayoManager()), k->actionManager, "papagayo");
}

void TupDocumentView::createLateralToolBar()
{
    k->toolbar = new QToolBar(tr("Draw tools"), this);
    k->toolbar->setIconSize(QSize(16, 16));
    addToolBar(Qt::LeftToolBarArea, k->toolbar);

    connect(k->toolbar, SIGNAL(actionTriggered(QAction *)), this, SLOT(selectToolFromMenu(QAction *)));

    // Brushes menu
    k->shapesMenu = new QMenu(tr("Brushes"), k->toolbar);
    k->shapesMenu->setIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "square.png"));
    connect(k->shapesMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    // Motion Tween menu
    k->motionMenu = new QMenu(tr("Tweening"), k->toolbar);
    k->motionMenu->setIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "tweening.png"));
    connect(k->motionMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));
}

void TupDocumentView::loadPlugins()
{
    bool imagePluginLoaded = false; 
    foreach (QObject *plugin, TupPluginManager::instance()->formats()) {
             if (plugin) {
                 TupExportInterface *exporter = qobject_cast<TupExportInterface *>(plugin);
                 if (exporter) {
                     #ifdef K_DEBUG
                         QString msg = "TupDocumentView::loadPlugins() - plugin: " + exporter->key();
                         #ifdef Q_OS_WIN32
                             qWarning() << msg;
                         #else
                             tWarning() << msg;
                         #endif
                     #endif

                     if (exporter->key().compare(tr("Image Array")) == 0) {
                         k->imagePlugin = exporter;
                         imagePluginLoaded = true;
                         break;
                     }
                 }
             }
    }

    if (!imagePluginLoaded) {
        #ifdef K_DEBUG
            QString msg = "TupDocumentView::loadPlugins() - Warning: Couldn't found plugin -> " + tr("Image Array");
            #ifdef Q_OS_WIN32
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif
    }

    QVector<TAction*> brushTools(3);
    QVector<TAction*> tweenTools(7);

    foreach (QObject *plugin, TupPluginManager::instance()->tools()) {
             TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(plugin);

             if (tool->toolType() != TupToolInterface::Tweener && tool->toolType() != TupToolInterface::LipSync) {
                 connect(tool, SIGNAL(closeHugeCanvas()), this, SLOT(closeFullScreen()));
                 connect(tool, SIGNAL(callForPlugin(int, int)), this, SLOT(loadPlugin(int, int)));
             }

             QStringList::iterator it;
             QStringList keys = tool->keys();

             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                      QString msg = "TupDocumentView::loadPlugins() - Tool Loaded: " + *it;
                      #ifdef Q_OS_WIN32
                          qWarning() << msg;
                      #else
                          tWarning() << msg;
                      #endif
                  #endif

                  TAction *action = tool->actions()[*it];

                  if (action) {
                      action->setIconVisibleInMenu(true);
                      connect(action, SIGNAL(triggered()), this, SLOT(selectTool()));
                      action->setParent(plugin);
                      QString toolName = action->text();

                      switch (tool->toolType()) {
                              case TupToolInterface::Brush:
                                 {
                                   // SQA: Experimental plugin (enable it only for testing)
                                   /*
                                   if (toolName.compare(tr("Scheme")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[1] = action;
                                   }
                                   */

                                   if (toolName.compare(tr("Pencil")) == 0)
                                       k->pencilAction = action;

                                   if (toolName.compare(tr("Ink")) == 0)
                                       k->inkAction = action;

                                   // SQA: This code has been disabled temporarily
                                   /*
                                   if (toolName.compare(tr("Eraser")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[2] = action;
                                   }
                                   */

                                   if (toolName.compare(tr("PolyLine")) == 0) {
                                       k->polyLineAction = action;
                                       TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(action->parent());
                                       connect(k->paintArea, SIGNAL(closePolyLine()), tool, SLOT(initEnv()));
                                       connect(this, SIGNAL(closePolyLine()), tool, SLOT(initEnv()));
                                   }

                                   if (toolName.compare(tr("Line")) == 0) {
                                       brushTools[2] = action;
                                       TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(action->parent());
                                       connect(k->paintArea, SIGNAL(closeLine()), tool, SLOT(endItem()));
                                       connect(this, SIGNAL(closeLine()), tool, SLOT(endItem()));
                                   }

                                   if (toolName.compare(tr("Rectangle")) == 0) {
                                       brushTools[0] = action;
                                       k->shapesMenu->setDefaultAction(action);
                                   }

                                   if (toolName.compare(tr("Ellipse")) == 0)
                                       brushTools[1] = action;

                                   // SQA: This code has been disabled temporarily
                                   /*
                                   if (toolName.compare(tr("Text")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[7] = action;
                                   }
                                   */
                                 }
                                 break;
                              case TupToolInterface::Tweener:
                                 {
                                   if (toolName.compare(tr("Position Tween")) == 0) {
                                       tweenTools[0] = action;
                                       k->motionMenu->setDefaultAction(action);
                                   }

                                   if (toolName.compare(tr("Rotation Tween")) == 0)
                                       tweenTools[1] = action;

                                   if (toolName.compare(tr("Scale Tween")) == 0)
                                       tweenTools[2] = action;

                                   if (toolName.compare(tr("Shear Tween")) == 0)
                                       tweenTools[3] = action;

                                   if (toolName.compare(tr("Opacity Tween")) == 0)
                                       tweenTools[4] = action;

                                   if (toolName.compare(tr("Coloring Tween")) == 0)
                                       tweenTools[5] = action;

                                   if (toolName.compare(tr("Compound Tween")) == 0)
                                       action->setDisabled(true);
                                       tweenTools[6] = action;
                                 }
                                 break;
                              case TupToolInterface::Selection:
                                 {
                                   if (toolName.compare(tr("Object Selection")) == 0)
                                       k->selectionAction = action;

                                   if (toolName.compare(tr("Nodes Selection")) == 0)
                                       k->nodesAction = action;
                                 }
                                 break;
                              case TupToolInterface::Fill:
                                 {
                                   if (toolName.compare(tr("Internal fill")) == 0)
                                       k->fillAction = action;

                                   if (toolName.compare(tr("Line fill")) == 0)
                                       k->borderFillAction = action;
                                 }
                                 break;
                               case TupToolInterface::View:
                                 {
                                   if (toolName.compare(tr("Hand")) == 0)
                                       k->handAction = action;
                                 }
                                 break;
                               case TupToolInterface::LipSync:
                                 {
                                   if (toolName.compare(tr("Papagayo Lip-sync")) == 0)
                                       k->papagayoAction = action;
                                 }
                                 break;
                               default:
                                 break;
                      }
                  } else {
                      #ifdef K_DEBUG
                          QString msg = "TupDocumentView::loadPlugins() - Fatal Error: Tool action is NULL -> " + *it;
                          #ifdef Q_OS_WIN32
                              qDebug() << msg;
                          #else
                              tError() << msg;
                          #endif
                      #endif
                  }
             }
    } // end foreach

    for (int i = 0; i < brushTools.size(); ++i) 
         k->shapesMenu->addAction(brushTools.at(i));

    for (int i = 0; i < tweenTools.size(); ++i)
         k->motionMenu->addAction(tweenTools.at(i));

    foreach (QObject *plugin, TupPluginManager::instance()->filters()) {
             AFilterInterface *filterInterface = qobject_cast<AFilterInterface *>(plugin);
             QStringList::iterator it;
             QStringList keys = filterInterface->keys();

             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                      QString msg = "TupDocumentView::loadPlugins() - Filter Loaded: " + *it;
                      #ifdef Q_OS_WIN32
                          qDebug() << msg;
                      #else
                          tDebug("plugins") << msg;
                      #endif
                  #endif

                  TAction *filter = filterInterface->actions()[*it];
                  if (filter) {
                      connect(filter, SIGNAL(triggered()), this, SLOT(applyFilter()));
                      k->filterMenu->addAction(filter);
                  }
             }
    }

    k->toolbar->addAction(k->pencilAction);
    k->toolbar->addAction(k->inkAction);
    k->toolbar->addAction(k->polyLineAction);
    k->toolbar->addSeparator();
    k->toolbar->addAction(k->shapesMenu->menuAction());
    k->toolbar->addSeparator();
    k->toolbar->addAction(k->selectionAction);
    k->toolbar->addAction(k->nodesAction);
    k->toolbar->addSeparator();
    k->toolbar->addAction(k->fillAction);
    k->toolbar->addAction(k->borderFillAction);
    k->toolbar->addSeparator();
    k->toolbar->addAction(k->handAction);
    k->toolbar->addSeparator();
    k->toolbar->addAction(k->motionMenu->menuAction());

    brushTools.clear();
    tweenTools.clear();

    k->pencilAction->trigger();
}

void TupDocumentView::loadPlugin(int menu, int index)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupDocumentView::loadPlugin()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TAction *action = 0;

    switch (menu) {
            case TupToolPlugin::Arrows:
                 {
                     if (index == TupToolPlugin::FrameBack) {
                         k->paintArea->goOneFrameBack();
                     } else if (index == TupToolPlugin::FrameForward) {
                                k->paintArea->goOneFrameForward();
                     } else if (index == TupToolPlugin::QuickCopy) {
                                k->paintArea->quickCopy();
                     }
                     return;
                 }
            break;
            case TupToolPlugin::ColorMenu:
                 {
                     if (index == TupToolPlugin::ColorTool) {
                         if (k->fullScreenOn) {
                             QColor currentColor = brushManager()->penColor();
                             emit openColorDialog(currentColor);
                         }
                         return;
                     }
                 }
            break;
            case TupToolPlugin::BrushesMenu:
                 {
                     QList<QAction*> brushActions = k->shapesMenu->actions();

                     switch (index) {
                             case TupToolPlugin::PencilTool:
                             {
                                 action = k->pencilAction;
                             }
                             break;
                             case TupToolPlugin::InkTool:
                             {
                                 action = k->inkAction;
                             }
                             break;
                             case TupToolPlugin::PolyLineTool:
                             {
                                 action = k->polyLineAction;
                             }
                             break;
                             case TupToolPlugin::RectangleTool:
                             {
                                 action = (TAction *) brushActions[0];
                             }
                             break;
                             case TupToolPlugin::EllipseTool:
                             {
                                 action = (TAction *) brushActions[1];
                             }
                             break;
                             case TupToolPlugin::LineTool:
                             {
                                 action = (TAction *) brushActions[2];
                             }
                             break;
                     }
                 }
            break;
            case TupToolPlugin::SelectionMenu:
                 {
                     switch (index) {
                         case TupToolPlugin::Delete:
                         {
                             k->paintArea->deleteItems();
                         }
                         break;
                         case TupToolPlugin::NodesTool:
                         {
                             action = k->nodesAction;
                         }
                         break;
                         case TupToolPlugin::ObjectsTool:
                         {
                             action = k->selectionAction;
                         }
                         break;
                     }
                 }
            break;
            case TupToolPlugin::FillMenu:
                 {
                     if (index == TupToolPlugin::InsideTool)
                         action = k->fillAction;

                     if (index == TupToolPlugin::ContourTool)
                         action = k->borderFillAction;
                 }
            break;
            case TupToolPlugin::ZoomMenu:
                 {
                     action = k->handAction;
                 }
            break;
            default:
                 {
                     #ifdef K_DEBUG
                         QString msg = "TupDocumentView::loadPlugin() - Error: Invalid Menu Index / No plugin loaded";
                         #ifdef Q_OS_WIN32
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                     return;
                 }
            break;
    }

    if (action) {
        QString toolName = tr("%1").arg(action->text());

        if (toolName.compare(k->currentTool->name()) != 0) {
            if (k->fullScreenOn) {
                action->trigger();
                k->fullScreen->updateCursor(action->cursor());
            }
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupDocumentView::loadPlugin() - Error: Action pointer is NULL!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }
}

void TupDocumentView::selectTool()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupDocumentView::selectTool()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TAction *action = qobject_cast<TAction *>(sender());

    if (action) {
        QString toolName = tr("%1").arg(action->text());

        if (k->currentTool) {
            if (toolName.compare(k->currentTool->name()) == 0)
                return;

            k->currentTool->saveConfig();
            QWidget *toolConfigurator = k->currentTool->configurator();
            if (toolConfigurator)
                k->configurationArea->close();
        }

        TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(action->parent());
        k->currentTool = tool; 
        tool->setName(toolName);
        k->paintArea->setCurrentTool(toolName);

        if (!action->icon().isNull())
            k->status->updateTool(toolName, action->icon().pixmap(15, 15));

        int minWidth = 0;

        switch (tool->toolType()) {
                case TupToolInterface::Brush: 
                     k->status->enableFullScreenFeature(true);
                     if (toolName.compare(tr("Pencil"))==0 || toolName.compare(tr("PolyLine"))==0) {
                         minWidth = 130;
                     } else if (toolName.compare(tr("Text"))==0) {
                                minWidth = 350;
                     } 

                     if (toolName.compare(tr("Rectangle"))==0 || toolName.compare(tr("Ellipse"))==0 || toolName.compare(tr("Line"))==0) { 
                         minWidth = 130;
                         k->shapesMenu->setDefaultAction(action);
                         k->shapesMenu->setActiveAction(action);
                         if (!action->icon().isNull())
                             k->shapesMenu->menuAction()->setIcon(action->icon());
                     }
                     break;
                     
                case TupToolInterface::Tweener:
                     k->status->enableFullScreenFeature(false);
                     minWidth = 220;
                     k->motionMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->motionMenu->menuAction()->setIcon(action->icon());
                     break;
                case TupToolInterface::Fill:
                     k->status->enableFullScreenFeature(true);
                     break;
                case TupToolInterface::Selection:
                     k->status->enableFullScreenFeature(true);
                     if (toolName.compare(tr("Object Selection"))==0) {
                         minWidth = 130;
                         connect(k->paintArea, SIGNAL(itemAddedOnSelection(TupGraphicsScene *)), 
                                 tool, SLOT(updateItems(TupGraphicsScene *)));
                     } 
                     break;
                case TupToolInterface::View:
                     k->status->enableFullScreenFeature(true);
                     if (toolName.compare(tr("Zoom In"))==0 || toolName.compare(tr("Zoom Out"))==0)
                         minWidth = 130;

                     if (toolName.compare(tr("Hand"))==0) {
                         tool->setProjectSize(k->project->dimension());
                         if (k->fullScreenOn)
                             tool->setActiveView("FULL_SCREEN");
                         else
                             tool->setActiveView("WORKSPACE");
                     }
                     break;
                case TupToolInterface::LipSync:
                     k->status->enableFullScreenFeature(false);
                     minWidth = 220;
                     connect(k->currentTool, SIGNAL(importLipSync()), this, SLOT(importPapagayoLipSync()));
                     break;
                default:
                     break;
        }

        QWidget *toolConfigurator = tool->configurator();

        if (toolConfigurator) {
            k->configurationArea = new TupConfigurationArea(this);
            k->configurationArea->setConfigurator(toolConfigurator, minWidth);
            addDockWidget(Qt::RightDockWidgetArea, k->configurationArea);
            toolConfigurator->show();
            if (!k->configurationArea->isVisible())
                k->configurationArea->show();
        } else {
            if (k->configurationArea->isVisible())
                k->configurationArea->close();
        }

        k->paintArea->setTool(tool);
        k->paintArea->viewport()->setCursor(action->cursor());

        if (toolName.compare(tr("Object Selection"))==0 || toolName.compare(tr("Nodes Selection"))==0 || toolName.compare(tr("PolyLine"))==0)
            tool->updateZoomFactor(1 / k->nodesScaleFactor);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupDocumentView::selectTool() - Fatal Error: Action from sender() is NULL";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupDocumentView::selectToolFromMenu(QAction *action)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupDocumentView::selectToolFromMenu()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QMenu *menu = qobject_cast<QMenu *>(action->parent());

    if (menu) {
        TAction *tool = qobject_cast<TAction *>(menu->activeAction());

        if (tool) {
            if (tool->text().compare(k->currentTool->name()) == 0)
                return;
            tool->trigger(); // this line calls selectTool()
        } else {
            tool = qobject_cast<TAction *>(menu->defaultAction());
            if (tool) {
                tool->trigger();
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupDocumentView::selectToolFromMenu() - Default action is NULL";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupDocumentView::selectToolFromMenu() - Error: Action with NO parent! Aborting...";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
    } 
}

bool TupDocumentView::handleProjectResponse(TupProjectResponse *event)
{
    return k->paintArea->handleResponse(event);
}

void TupDocumentView::applyFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        // SQA: Check this code to understand how filters work 
        /*
        SQA issue: Pending to check this code  
        AFilterInterface *aFilter = qobject_cast<AFilterInterface *>(action->parent());
        QString filter = action->text();
        
        TupFrame *frame = k->paintArea->currentFrame();
        if (frame) {
            aFilter->filter(action->text(), frame->components());
            k->paintArea->redrawAll();
        }
        */
    }
}

void TupDocumentView::createToolBar()
{
    k->barGrid = new QToolBar(tr("Paint area actions"), this);
    k->barGrid->setIconSize(QSize(16, 16));

    k->propertiesBar = new QToolBar(tr("Dynamic Background Properties"), this);

    addToolBar(k->barGrid);

    k->spaceMode = new QComboBox();
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons" + QDir::separator() + "frames_mode.png"), tr("Frames Mode"));
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons" + QDir::separator() + "static_background_mode.png"), tr("Static BG Mode"));
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons" + QDir::separator() + "dynamic_background_mode.png"), tr("Dynamic BG Mode"));

    connect(k->spaceMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSpaceContext()));
    setSpaceContext();
    k->barGrid->addWidget(k->spaceMode);

    k->barGrid->addAction(kApp->findGlobalAction("undo"));
    k->barGrid->addAction(kApp->findGlobalAction("redo"));

    k->barGrid->addAction(k->actionManager->find("copy"));
    k->barGrid->addAction(k->actionManager->find("paste"));
    k->barGrid->addAction(k->actionManager->find("cut"));
    k->barGrid->addAction(k->actionManager->find("delete"));

    k->barGrid->addAction(k->actionManager->find("group"));
    k->barGrid->addAction(k->actionManager->find("ungroup"));

    TCONFIG->beginGroup("OnionParameters");
    int preview = TCONFIG->value("PreviousFrames", -1).toInt();
    int next = TCONFIG->value("NextFrames", -1).toInt();

    k->prevOnionSkinSpin = new QSpinBox(this);
    k->prevOnionSkinSpin->setToolTip(tr("Previous Frames"));
    connect(k->prevOnionSkinSpin, SIGNAL(valueChanged(int)), this, SLOT(setPreviousOnionSkin(int)));

    if (preview > 0)
        k->prevOnionSkinSpin->setValue(preview);
    else
        k->prevOnionSkinSpin->setValue(1);

    k->barGrid->addWidget(k->prevOnionSkinSpin);
    k->barGrid->addAction(k->actionManager->find("onion"));

    k->nextOnionSkinSpin = new QSpinBox(this);
    k->nextOnionSkinSpin->setToolTip(tr("Next Frames"));
    connect(k->nextOnionSkinSpin, SIGNAL(valueChanged (int)), this, SLOT(setNextOnionSkin(int)));

    if (next > 0)
        k->nextOnionSkinSpin->setValue(next);
    else
        k->nextOnionSkinSpin->setValue(1);

    k->barGrid->addWidget(k->nextOnionSkinSpin);

    k->barGrid->addAction(k->actionManager->find("onionfactor"));

    k->onionFactorSpin = new QDoubleSpinBox(this);
    k->onionFactorSpin->setRange(0, 1);
    k->onionFactorSpin->setSingleStep(0.01);
    k->onionFactorSpin->setValue(k->opacityFactor);
    k->onionFactorSpin->setToolTip(tr("Onion Skin Factor"));
    connect(k->onionFactorSpin, SIGNAL(valueChanged(double)), this, SLOT(setOnionFactor(double)));

    k->barGrid->addWidget(k->onionFactorSpin);

    k->barGrid->addAction(k->actionManager->find("export_image"));

    TCONFIG->beginGroup("Network");
    QString server = TCONFIG->value("Server").toString();

    if (k->isNetworked && server.compare("tupitu.be") == 0)
        k->barGrid->addAction(k->actionManager->find("post_image"));

    k->barGrid->addAction(k->actionManager->find("storyboard"));

    k->barGrid->addAction(k->actionManager->find("camera"));

    k->barGrid->addAction(k->actionManager->find("papagayo"));

    addToolBarBreak();

    QLabel *dirLabel = new QLabel(tr("Direction") + ": ");

    k->dirCombo = new QComboBox;
    k->dirCombo->addItem(tr("Left to Right"));
    k->dirCombo->addItem(tr("Right to Left"));
    k->dirCombo->addItem(tr("Top to Bottom"));
    k->dirCombo->addItem(tr("Bottom to Top"));
    connect(k->dirCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setBackgroundDirection(int)));

    QLabel *shiftLabel = new QLabel(tr("Shift Length") + ": ");

    k->shiftSpin = new QSpinBox(this);
    k->shiftSpin->setSingleStep(1);
    k->shiftSpin->setRange(1, 1000);
    connect(k->shiftSpin, SIGNAL(valueChanged(int)), this, SLOT(updateBackgroundShiftProperty(int)));

    QWidget *empty1 = new QWidget();
    empty1->setFixedWidth(5);
    QWidget *empty2 = new QWidget();
    empty2->setFixedWidth(5);

    k->propertiesBar->addWidget(dirLabel);
    k->propertiesBar->addWidget(k->dirCombo);
    k->propertiesBar->addWidget(empty1);
    k->propertiesBar->addSeparator();
    k->propertiesBar->addWidget(empty2);
    k->propertiesBar->addWidget(shiftLabel);
    k->propertiesBar->addWidget(k->shiftSpin);

    k->propertiesBar->setVisible(false);

    addToolBar(k->propertiesBar);
}

void TupDocumentView::closeArea()
{
    if (k->configurationArea->isVisible())
        k->configurationArea->close();

    k->paintArea->setScene(0);
    close();
}

void TupDocumentView::undo()
{
    puts("Adding undo support");
}

void TupDocumentView::redo()
{
    puts("Adding redo support");
}

void TupDocumentView::setCursor(const QCursor &cursor)
{
    Q_UNUSED(cursor);
 /*
    k->paintArea->setCursor(c);
 */
}

void TupDocumentView::setPreviousOnionSkin(int level)
{
    TCONFIG->beginGroup("OnionParameters");
    TCONFIG->setValue("PreviousFrames", level);

    k->paintArea->setPreviousFramesOnionSkinCount(level);
}

void TupDocumentView::setNextOnionSkin(int level)
{
    TCONFIG->beginGroup("OnionParameters");
    TCONFIG->setValue("NextFrames", level);

    k->paintArea->setNextFramesOnionSkinCount(level);
}

void TupDocumentView::changeRulerOrigin(const QPointF &zero)
{
    k->verticalRuler->setOrigin(zero.y());
    k->horizontalRuler->setOrigin(zero.x());
}

QSize TupDocumentView::sizeHint() const
{
    QSize size(parentWidget()->size());

    return size.expandedTo(QApplication::globalStrut());
}

QSize TupDocumentView::workSpaceSize() const
{
    return k->paintArea->size();
}

TupBrushManager *TupDocumentView::brushManager() const
{
    return k->paintArea->brushManager();
}

TupPaintAreaCommand *TupDocumentView::createCommand(const TupPaintAreaEvent *event)
{
    TupPaintAreaCommand *command = new TupPaintAreaCommand(k->paintArea, event);

    return command;
}

void TupDocumentView::updatePaintArea()
{
    k->paintArea->updatePaintArea(); 
}

void TupDocumentView::callAutoSave()
{
    emit autoSave();
}

void TupDocumentView::saveTimer()
{
    TCONFIG->beginGroup("General");
    k->autoSaveTime = TCONFIG->value("AutoSave", 10).toInt();

    k->timer = new QTimer(this);

    if (k->autoSaveTime != 0) {
        if (k->autoSaveTime < 0 || k->autoSaveTime > 60) 
            k->autoSaveTime = 5;

        int saveTime = k->autoSaveTime*60000;
        connect(k->timer, SIGNAL(timeout()), this, SLOT(callAutoSave()));
        k->timer->start(saveTime);
    }
}

void TupDocumentView::setSpaceContext()
{
    TupProject::Mode mode = TupProject::Mode(k->spaceMode->currentIndex());

    if (mode == TupProject::FRAMES_EDITION) {
        if (k->dynamicFlag) {
            k->dynamicFlag = false;
            renderDynamicBackground();
        }
        k->project->updateSpaceContext(TupProject::FRAMES_EDITION);
        k->propertiesBar->setVisible(false);
        k->motionMenu->setEnabled(true);
    } else if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
               if (k->dynamicFlag) {
                   k->dynamicFlag = false;
                   renderDynamicBackground();
               }
               k->project->updateSpaceContext(TupProject::STATIC_BACKGROUND_EDITION);
               k->propertiesBar->setVisible(false);
               k->motionMenu->setEnabled(false);
    } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
               k->dynamicFlag = true;
               k->project->updateSpaceContext(TupProject::DYNAMIC_BACKGROUND_EDITION);

               int sceneIndex = k->paintArea->currentSceneIndex();
               TupScene *scene = k->project->scene(sceneIndex);
               if (scene) {
                   TupBackground *bg = scene->background();
                   if (bg) {
                       int direction = bg->dyanmicDirection();
                       k->dirCombo->setCurrentIndex(direction);
                       int shift = bg->dyanmicShift();
                       k->shiftSpin->setValue(shift);
                   }
               }
               k->propertiesBar->setVisible(true);
               k->motionMenu->setEnabled(false);
    }

    k->paintArea->updateSpaceContext();
    k->paintArea->updatePaintArea();

   if (k->currentTool) {
       k->currentTool->init(k->paintArea->graphicsScene()); 
       if (((k->currentTool->toolType() == TupToolInterface::Tweener) || (k->currentTool->toolType() == TupToolInterface::LipSync))
           && (mode != TupProject::FRAMES_EDITION)) {
           k->pencilAction->trigger();
       }
   }

   emit modeHasChanged(mode);
}

TupProject::Mode TupDocumentView::spaceContext()
{
   return TupProject::Mode(k->spaceMode->currentIndex());
}

TupProject *TupDocumentView::project()
{
   return k->project;
}

int TupDocumentView::currentFramesTotal()
{
   int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();
   int layerIndex = k->paintArea->graphicsScene()->currentLayerIndex();

   TupScene *scene = k->project->scene(sceneIndex);

   if (scene) {
       TupLayer *layer = scene->layer(layerIndex);
       if (layer)
           return layer->framesTotal();
    }

    return -1;
}

int TupDocumentView::currentSceneIndex()
{
    if (k->paintArea)
        return k->paintArea->graphicsScene()->currentSceneIndex();
   
    return -1; 
}

void TupDocumentView::updateBgColor(const QColor color)
{
   if (!k->isNetworked) {
       k->project->setBgColor(color);
       k->paintArea->setBgColor(color);
   } else {
       TupProjectRequest event = TupRequestBuilder::createSceneRequest(currentSceneIndex(), TupProjectRequest::BgColor, color.name());
       emit requestTriggered(&event);
   }
}

void TupDocumentView::enableOnionFeature()
{
    if (!k->onionEnabled) {
        if (k->prevOnionValue == 0)
            k->prevOnionSkinSpin->setValue(1);
        else
            k->prevOnionSkinSpin->setValue(k->prevOnionValue);

        if (k->nextOnionValue == 0)
            k->nextOnionSkinSpin->setValue(1);
        else
            k->nextOnionSkinSpin->setValue(k->nextOnionValue);

        k->onionEnabled = true;
    } else {
        k->prevOnionValue = k->prevOnionSkinSpin->value();
        k->nextOnionValue = k->nextOnionSkinSpin->value();
        k->prevOnionSkinSpin->setValue(0);
        k->nextOnionSkinSpin->setValue(0);
        k->onionEnabled = false;
    }

    k->paintArea->updatePaintArea();
}

void TupDocumentView::setDefaultOnionFactor()
{
    k->onionFactorSpin->setValue(0.5);
    setOnionFactor(0.5);
}

void TupDocumentView::setOnionFactor(double opacity)
{
    TCONFIG->beginGroup("OnionParameters");
    TCONFIG->setValue("OnionFactor", opacity);

    k->paintArea->setOnionFactor(opacity);
}

void TupDocumentView::showFullScreen()
{
    if (k->fullScreenOn || k->currentTool->toolType() == TupToolInterface::Tweener || k->currentTool->toolType() == TupToolInterface::LipSync)
        return;

    k->fullScreenOn = true;

    QDesktopWidget desktop;
    int screenW = desktop.screenGeometry().width();
    int screenH = desktop.screenGeometry().height();

    k->cacheScaleFactor = k->nodesScaleFactor;

    qreal scaleFactor = 1;

    QSize projectSize = k->project->dimension();
    if (projectSize.width() < projectSize.height())
        scaleFactor = (double) (screenW - 50) / (double) projectSize.width();
    else
        scaleFactor = (double) (screenH - 50) / (double) projectSize.height();

    k->fullScreen = new TupCanvas(this, Qt::Window|Qt::FramelessWindowHint, k->paintArea->graphicsScene(), 
                                 k->paintArea->centerPoint(), QSize(screenW, screenH), k->project, scaleFactor,
                                 k->viewAngle, brushManager(), k->isNetworked, k->onLineUsers); 

    k->fullScreen->updateCursor(k->currentTool->cursor());

    QString toolName = k->currentTool->name();
    if (toolName.compare(tr("Hand")) == 0)
        k->currentTool->setActiveView("FULL_SCREEN");

    k->fullScreen->showFullScreen();
    k->nodesScaleFactor = 1;
    updateNodesScale(scaleFactor);

    connect(this, SIGNAL(openColorDialog(const QColor &)), k->fullScreen, SLOT(colorDialog(const QColor &)));
    connect(k->fullScreen, SIGNAL(updateColorFromFullScreen(const QColor &)), this, SIGNAL(updateColorFromFullScreen(const QColor &)));
    connect(k->fullScreen, SIGNAL(updatePenThicknessFromFullScreen(int)), this, SLOT(updatePenThickness(int)));
    connect(k->fullScreen, SIGNAL(updateOnionOpacityFromFullScreen(double)), this, SLOT(updateOnionOpacity(double)));
    connect(k->fullScreen, SIGNAL(updateZoomFactorFromFullScreen(qreal)), this, SLOT(updateNodesScale(qreal)));
    connect(k->fullScreen, SIGNAL(callAction(int, int)), this, SLOT(loadPlugin(int, int)));
    connect(k->fullScreen, SIGNAL(requestTriggered(const TupProjectRequest *)), this, SIGNAL(requestTriggered(const TupProjectRequest *)));
    connect(k->fullScreen, SIGNAL(localRequestTriggered(const TupProjectRequest *)), this, SIGNAL(localRequestTriggered(const TupProjectRequest *)));
    connect(k->fullScreen, SIGNAL(rightClick()), this, SLOT(fullScreenRightClick()));

    connect(k->fullScreen, SIGNAL(goToFrame(int, int, int)), this, SLOT(selectFrame(int, int, int)));
    connect(k->fullScreen, SIGNAL(goToScene(int)), this, SLOT(selectScene(int)));
}

void TupDocumentView::updatePenThickness(int size) 
{
    QPen pen = brushManager()->pen();
    pen.setWidth(size);
    emit updatePenFromFullScreen(pen);
}

void TupDocumentView::updateOnionOpacity(double opacity)
{
    k->paintArea->setOnionFactor(opacity);
    k->onionFactorSpin->setValue(opacity);
}

void TupDocumentView::closeFullScreen()
{
    if (k->fullScreenOn) {
        disconnect(this, SIGNAL(openColorDialog(const QColor &)), k->fullScreen, SLOT(colorDialog(const QColor &)));
        disconnect(k->fullScreen, SIGNAL(updateColorFromFullScreen(const QColor &)), this, SIGNAL(updateColorFromFullScreen(const QColor &)));
        disconnect(k->fullScreen, SIGNAL(updatePenThicknessFromFullScreen(int)), this, SLOT(updatePenThickness(int))); 
        disconnect(k->fullScreen, SIGNAL(updateOnionOpacityFromFullScreen(double)), this, SLOT(updateOnionOpacity(double)));
        disconnect(k->fullScreen, SIGNAL(updateZoomFactorFromFullScreen(qreal)), this, SLOT(updateNodesScale(qreal)));
        disconnect(k->fullScreen, SIGNAL(callAction(int, int)), this, SLOT(loadPlugin(int, int)));
        disconnect(k->fullScreen, SIGNAL(requestTriggered(const TupProjectRequest *)), this, SIGNAL(requestTriggered(const TupProjectRequest *)));
        disconnect(k->fullScreen, SIGNAL(localRequestTriggered(const TupProjectRequest *)), this, SIGNAL(localRequestTriggered(const TupProjectRequest *)));

        disconnect(k->fullScreen, SIGNAL(goToFrame(int, int, int)), this, SLOT(selectFrame(int, int, int)));
        disconnect(k->fullScreen, SIGNAL(goToScene(int)), this, SLOT(selectScene(int)));

        k->fullScreen->close();
        k->fullScreenOn = false;
        k->currentTool->init(k->paintArea->graphicsScene());
        k->fullScreen = 0;

        QString toolName = k->currentTool->name();
        if (toolName.compare(tr("Hand")) == 0) 
            k->currentTool->setActiveView("WORKSPACE");

        k->nodesScaleFactor = k->cacheScaleFactor;
        updateNodesScale(1);
    }
}

void TupDocumentView::selectFrame(int frame, int layer, int scene)
{
    k->paintArea->goToFrame(frame, layer, scene);
}

void TupDocumentView::selectScene(int scene)
{
    k->paintArea->goToScene(scene);
}

void TupDocumentView::exportImage()
{
    int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();
    int frameIndex = k->paintArea->graphicsScene()->currentFrameIndex();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Frame As"), QDir::homePath(),
                                                        tr("Images") + " (*.png *.jpg)");
    if (!fileName.isNull()) {
        bool isOk = k->imagePlugin->exportFrame(frameIndex, k->project->bgColor(), fileName, k->project->scene(sceneIndex), k->project->dimension(), k->project->library()); 
        updatePaintArea();
        if (isOk)
            TOsd::self()->display(tr("Information"), tr("Frame has been exported successfully"));
        else
            TOsd::self()->display(tr("Error"), tr("Can't export frame as image"), TOsd::Error);
    }
}

void TupDocumentView::postImage()
{
    int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();
    int frameIndex = k->paintArea->graphicsScene()->currentFrameIndex();

    TupImageDialog *dialog = new TupImageDialog(this);
    dialog->show();
    QDesktopWidget desktop;
    dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                 (int) (desktop.screenGeometry().height() - dialog->height())/2);

    if (dialog->exec() != QDialog::Rejected) {
        QString title = dialog->imageTitle();
        QString topics = dialog->imageTopics();
        QString description = dialog->imageDescription();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        emit requestExportImageToServer(frameIndex, sceneIndex, title, topics, description);
    }
}

void TupDocumentView::updateStatusBgColor(const QColor color)
{
    k->status->setBgColor(color);
}

void TupDocumentView::storyboardSettings()
{
    QDesktopWidget desktop;
    int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    TupStoryBoardDialog *storySettings = new TupStoryBoardDialog(k->isNetworked, k->imagePlugin, k->project->bgColor(), k->project->dimension(), 
                                                                 k->project->scene(sceneIndex), currentSceneIndex(), k->project->library(), this);
    connect(storySettings, SIGNAL(updateStoryboard(TupStoryboard *, int)), this, SLOT(sendStoryboard(TupStoryboard *, int)));

    if (k->isNetworked)
        connect(storySettings, SIGNAL(postStoryboard(int)), this, SIGNAL(postStoryboard(int)));

    QApplication::restoreOverrideCursor();

    storySettings->show();
    storySettings->move((int) (desktop.screenGeometry().width() - storySettings->width())/2 ,
                        (int) (desktop.screenGeometry().height() - storySettings->height())/2);
}

void TupDocumentView::sendStoryboard(TupStoryboard *storyboard, int sceneIndex)
{
    if (k->isNetworked) {
        #ifdef K_DEBUG
            QString msg = "TupDocumentView::sendStoryboard() - Sending storyboard...";
            #ifdef Q_OS_WIN32
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif
        emit updateStoryboard(storyboard, sceneIndex);
    } else {
        k->project->scene(sceneIndex)->setStoryboard(storyboard);    
    }
}

void TupDocumentView::updateUsersOnLine(const QString &login, int state)
{
    if (state == 1) {
        k->onLineUsers << login; 
    } else {
        int index = k->onLineUsers.indexOf(login);
        k->onLineUsers.removeAt(index); 
    }

    if (k->fullScreenOn)
        k->fullScreen->updateOnLineUsers(k->onLineUsers);
}

void TupDocumentView::setBackgroundDirection(int direction)
{
   int sceneIndex = k->paintArea->currentSceneIndex();
   TupScene *scene = k->project->scene(sceneIndex);
   if (scene) {
       TupBackground *bg = scene->background();
       if (bg) {
           bg->setDyanmicDirection(direction);
           // emit projectHasChanged();
       }
   }
}

void TupDocumentView::updateBackgroundShiftProperty(int shift)
{
   int sceneIndex = k->paintArea->currentSceneIndex();
   TupScene *scene = k->project->scene(sceneIndex);
   if (scene) {
       TupBackground *bg = scene->background();
       if (bg) {
           bg->setDyanmicShift(shift);
           // emit projectHasChanged();
       }
   }
}

void TupDocumentView::renderDynamicBackground()
{
   int sceneIndex = k->paintArea->currentSceneIndex();
   TupScene *scene = k->project->scene(sceneIndex); 

   if (scene) {
       TupBackground *bg = scene->background();
       if (bg) {
           bg->renderDynamicView();
       }
   }
}

void TupDocumentView::fullScreenRightClick()
{
   if (k->currentTool->name().compare(tr("PolyLine")) == 0)
       emit closePolyLine();

   if (k->currentTool->name().compare(tr("Line")) == 0)
       emit closeLine();
}

void TupDocumentView::cameraInterface()
{
    int camerasTotal = QCamera::availableDevices().count();
    if (camerasTotal > 0) {
        QList<QByteArray> cameraDevices;
        QComboBox *devicesCombo = new QComboBox;
        foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
                QCamera *device = new QCamera(deviceName);
                QString description = device->deviceDescription(deviceName);
                bool found = false;
                for (int i=0; i<devicesCombo->count(); i++) {
                     QString item = devicesCombo->itemText(i);
                     if (item.compare(description) == 0) {
                         found = true;
                         break;
                     }
                }
                if (!found) {
                    devicesCombo->addItem(description);
                    cameraDevices << deviceName;
                }
        }

        /* SQA: This lines should be enabled in some point at the future
        QByteArray cameraDevice = cameraDevices[0];
        QCamera *camera = new QCamera(cameraDevice);
        camera->load();
        tError() << "TupDocumentView::cameraInterface() - Camera status: " << camera->status();
        QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);
        QList<QSize> resolutions = imageCapture->supportedResolutions();
        */

        QList<QSize> resolutions;    
        resolutions << QSize(1280, 1024);
        resolutions << QSize(1280, 960);
        resolutions << QSize(1224, 768);
        resolutions << QSize(800, 600);
        resolutions << QSize(640, 480);
        resolutions << QSize(352, 288);
        resolutions << QSize(320, 240);
        resolutions << QSize(176, 144);
        resolutions << QSize(160, 120);

        QDesktopWidget desktop;
        QSize projectSize = k->project->dimension();

        TupCameraDialog *cameraDialog = new TupCameraDialog(devicesCombo, projectSize, resolutions);
        cameraDialog->show();
        cameraDialog->move((int) (desktop.screenGeometry().width() - cameraDialog->width())/2 ,
                           (int) (desktop.screenGeometry().height() - cameraDialog->height())/2);

        if (cameraDialog->exec() == QDialog::Accepted) {
            k->cameraSize = cameraDialog->cameraResolution();
            QString title = QString::number(k->cameraSize.width()) + "x" + QString::number(k->cameraSize.height());

            if (cameraDialog->changeProjectSize()) {
                if (k->cameraSize != projectSize) 
                    resizeProjectDimension(k->cameraSize);
            } 

            if (cameraDialog->useBasicCamera()) {
                TupBasicCameraInterface *dialog = new TupBasicCameraInterface(title, cameraDevices, devicesCombo, cameraDialog->cameraIndex(), k->cameraSize, k->photoCounter);

                connect(dialog, SIGNAL(pictureHasBeenSelected(int, const QString)), this, SLOT(insertPictureInFrame(int, const QString)));
                dialog->show();
                dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                             (int) (desktop.screenGeometry().height() - dialog->height())/2);
            } else {
                TupCameraInterface *dialog = new TupCameraInterface(title, cameraDevices, devicesCombo, cameraDialog->cameraIndex(),
                                                                k->cameraSize, k->photoCounter);

                connect(dialog, SIGNAL(pictureHasBeenSelected(int, const QString)), this, SLOT(insertPictureInFrame(int, const QString)));
                dialog->show();
                dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                             (int) (desktop.screenGeometry().height() - dialog->height())/2);
            }
        }
    } else {
        // No devices connected!
        TOsd::self()->display(tr("Error"), tr("No cameras detected"), TOsd::Error);
    }
}

void TupDocumentView::resizeProjectDimension(const QSize dimension)
{
    k->paintArea->updateDimension(dimension);

    int width = k->wsDimension.width();
    int height = k->wsDimension.height();
    int pWidth = dimension.width();
    int pHeight = dimension.height();

    double proportion = 1;

    if (pWidth > pHeight)
        proportion = (double) width / (double) pWidth;
    else
        proportion = (double) height / (double) pHeight;

    if (proportion <= 0.5) {
        setZoomPercent("20");
    } else if (proportion > 0.5 && proportion <= 0.75) {
               setZoomPercent("25");
    } else if (proportion > 0.75 && proportion <= 1.5) {
               setZoomPercent("50");
    } else if (proportion > 1.5 && proportion < 2) {
               setZoomPercent("75");
    }

    emit projectSizeHasChanged(dimension);
    k->paintArea->updatePaintArea();
}

void TupDocumentView::insertPictureInFrame(int id, const QString path)
{
    // SQA: This is a hack - remember to check the QImageEncoderSettings issue 
    QImage pixmap(path); 
    if (pixmap.size() != k->cameraSize) {
        int height = pixmap.height();
        int width = (k->cameraSize.width() * height) / k->cameraSize.height();
        int posX = (pixmap.width() - width)/2;
        int posY = 0;
        if (width > pixmap.width()) {
            width = pixmap.width();
            height = (k->cameraSize.height() * width) / k->cameraSize.width(); 
            posX = 0;
            posY = (pixmap.height() - height)/2;
        }
        QImage mask = pixmap.copy(posX, posY, width, height);
        QImage resized = mask.scaledToWidth(k->cameraSize.width(), Qt::SmoothTransformation);
        resized.save(path, "JPG", 100);
    } 

    QFile f(path);
    QFileInfo fileInfo(f);
    QString key = fileInfo.fileName().toLower();

    if (f.open(QIODevice::ReadOnly)) {
        if (id > 1) {
            int frameIndex = k->paintArea->currentFrameIndex() + 1;

            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->paintArea->currentSceneIndex(), k->paintArea->currentLayerIndex(), 
                                                                              frameIndex, TupProjectRequest::Add, tr("Frame %1").arg(frameIndex + 1));
            emit requestTriggered(&request);

            request = TupRequestBuilder::createFrameRequest(k->paintArea->currentSceneIndex(), k->paintArea->currentLayerIndex(), frameIndex,
                                                            TupProjectRequest::Select);
            emit requestTriggered(&request);
        } 

        QByteArray data = f.readAll();
        f.close();

        TupLibrary *library = k->project->library();
        while(library->exists(key)) {
              id++;
              QString prev = "pic";
              if (id < 10)
                  prev += "00";
              if (id >= 10 && id < 100)
                  prev += "0";
              key = prev + QString::number(id) + ".jpg";
        }

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, key,
                                                                            TupLibraryObject::Image, k->project->spaceContext(), data, QString(),
                                                                            k->paintArea->currentSceneIndex(), k->paintArea->currentLayerIndex(), 
                                                                            k->paintArea->currentFrameIndex());
        emit requestTriggered(&request);

        k->photoCounter = id + 1;
    }
}

void TupDocumentView::importPapagayoLipSync()
{
    TupPapagayoDialog *dialog = new TupPapagayoDialog();
    dialog->show();

    if (dialog->exec() != QDialog::Rejected) {
        QString file = dialog->getPGOFile();
        QFileInfo info(file);
        QString folder = info.fileName().toLower();

        int sceneIndex = k->paintArea->currentSceneIndex();
        TupScene *scene = k->project->scene(sceneIndex);
        if (scene->lipSyncExists(folder)) {
            TOsd::self()->display(tr("Error"), tr("Papagayo project already exists!\nPlease, rename the project's file"), TOsd::Error);
            #ifdef K_DEBUG
                   QString msg = "TupDocumentView::importPapagayoLipSync() - Fatal Error: Papagayo file is invalid!";
                   #ifdef Q_OS_WIN32
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
            #endif
            return;
        }

        QString imagesDir = dialog->getImagesFile();
        QFile project(file);
        if (project.exists()) {
            if (project.size() > 0) {
                QDir dir(imagesDir);
                QStringList imagesList = dir.entryList(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.gif" << "*.svg");
                if (imagesList.count() > 0) {
                    QSize mouthSize;
                    QString extension = ".svg";
                    QString firstImage = imagesList.at(0);
                    QString pic = imagesDir + QDir::separator() + firstImage;
                    if (firstImage.endsWith(".svg")) {
                        QSvgRenderer *renderer = new QSvgRenderer(pic);
                        QRect rect = renderer->viewBox();
                        mouthSize = rect.size();
                    } else {
                        QImage *image = new QImage(pic);
                        mouthSize = image->size();
                        int dot = firstImage.lastIndexOf(".");
                        extension = firstImage.mid(dot);
                    }

                    int currentIndex = k->paintArea->currentFrameIndex();
                    TupPapagayoImporter *parser = new TupPapagayoImporter(file, k->project->dimension(), mouthSize, extension, currentIndex);
                    if (parser->fileIsValid()) {
                        int layerIndex = k->paintArea->currentLayerIndex();
                        QString mouthPath = imagesDir;
                        QDir mouthDir = QDir(mouthPath);

                        // Creating Papagayo folder in the library
                        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, folder, TupLibraryObject::Folder);
                        emit requestTriggered(&request);

                        // Adding mouth images in the library
                        foreach (QString fileName, imagesList) {
                                 QString key = fileName.toLower();
                                 QFile f(mouthPath + QDir::separator() + fileName);
                                 if (f.open(QIODevice::ReadOnly)) {
                                     QByteArray data = f.readAll();
                                     f.close();
                                     request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, key, TupLibraryObject::Image, k->project->spaceContext(), data, folder,
                                                                                       sceneIndex, layerIndex, currentIndex);
                                     emit requestTriggered(&request);
                                 }
                        }

                        // Adding lip-sync sound file
                        QString soundFile = dialog->getSoundFile();
                        QFile f(soundFile);
                        QFileInfo info(soundFile);
                        QString soundKey = info.fileName().toLower();

                        if (f.open(QIODevice::ReadOnly)) {
                            QByteArray data = f.readAll();
                            f.close();
                            request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, soundKey, TupLibraryObject::Sound, k->project->spaceContext(), data, folder,
                                                                              sceneIndex, layerIndex, currentIndex);
                            emit requestTriggered(&request);
                        }

                        // Adding Papagayo project
                        parser->setSoundFile(soundKey);
                        QString xml = parser->file2Text();
                        request = TupRequestBuilder::createLayerRequest(sceneIndex, layerIndex, TupProjectRequest::AddLipSync, xml);
                        emit requestTriggered(&request);

                        // Adding frames if they are required
                        TupScene *scene = k->project->scene(sceneIndex);
                        if (scene) {
                            int sceneFrames = scene->framesTotal();
                            int lipSyncFrames = currentIndex + parser->framesTotal();

                            if (lipSyncFrames > sceneFrames) {
                                int layersTotal = scene->layersTotal();
                                for (int i = sceneFrames; i < lipSyncFrames; i++) {
                                     for (int j = 0; j < layersTotal; j++) {
                                          request = TupRequestBuilder::createFrameRequest(sceneIndex, j, i, TupProjectRequest::Add, tr("Frame %1").arg(i + 1));
                                          emit requestTriggered(&request);
                                     }
                                }
                                request = TupRequestBuilder::createFrameRequest(sceneIndex, layerIndex, currentIndex, TupProjectRequest::Select, "1");
                                emit requestTriggered(&request);
                            }
                        }

                        if (k->currentTool->name().compare(tr("Papagayo Lip-sync")) != 0)
                            k->papagayoAction->trigger();

                        emit updateFPS(parser->fps()); 

                        TOsd::self()->display(tr("Information"), tr("Papagayo file has been imported successfully"));
                    } else {
                        TOsd::self()->display(tr("Error"), tr("Papagayo file is invalid!"), TOsd::Error);
                        #ifdef K_DEBUG
                            QString msg = "TupDocumentView::importPapagayoLipSync() - Fatal Error: Papagayo file is invalid!";
                            #ifdef Q_OS_WIN32
                                qDebug() << msg;
                            #else
                                tError() << msg;
                            #endif
                        #endif
                    }
                } else {
                    TOsd::self()->display(tr("Error"), tr("Images directory is empty!"), TOsd::Error);
                    #ifdef K_DEBUG
                        QString msg = "TupDocumentView::importPapagayoLipSync() - Fatal Error: Images directory is empty!";
                        #ifdef Q_OS_WIN32
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            } else {
                TOsd::self()->display(tr("Error"), tr("Papagayo project is invalid!"), TOsd::Error);
                #ifdef K_DEBUG
                    QString msg = "TupDocumentView::importPapagayoLipSync() - Fatal Error: Papagayo file is invalid!";
                    #ifdef Q_OS_WIN32
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            }
        } else {
            TOsd::self()->display(tr("Error"), tr("Papagayo project is invalid!"), TOsd::Error);
            #ifdef K_DEBUG
                QString msg = "TupDocumentView::importPapagayoLipSync() - Fatal Error: Papagayo file doesn't exist!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    }
}

void TupDocumentView::papagayoManager()
{
    if (k->currentTool->name().compare(tr("Papagayo Lip-sync")) != 0) {
        TupProject::Mode mode = TupProject::Mode(k->spaceMode->currentIndex());
        if (mode != TupProject::FRAMES_EDITION)
            k->spaceMode->setCurrentIndex(TupProject::FRAMES_EDITION);
        k->papagayoAction->trigger();
    }
}

void TupDocumentView::updatePerspective()
{
    if (k->currentTool) {
        if (k->currentTool->name().compare(tr("Papagayo Lip-sync")) == 0)
            k->currentTool->updateWorkSpaceContext();
    }
}

void TupDocumentView::resetWorkSpaceTransformations()
{
    k->paintArea->resetWorkSpaceCenter(k->project->dimension());
    k->status->setRotationAngle("0");
    k->status->setZoomPercent("100");
}
