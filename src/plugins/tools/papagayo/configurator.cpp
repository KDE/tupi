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

#include "configurator.h"
#include "lipsyncmanager.h"
#include "buttonspanel.h"
#include "tosd.h"

struct Configurator::Private
{
    QBoxLayout *layout;
    QBoxLayout *settingsLayout;
    Settings *settingsPanel;
    LipSyncManager *manager;
    ButtonsPanel *controlPanel;

    TupLipSync *lipSync;

    int framesTotal;
    int currentFrame;

    TupToolPlugin::Mode mode;
    GuiState state;
};

Configurator::Configurator(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->framesTotal = 1;
    k->currentFrame = 0;

    k->mode = TupToolPlugin::View;
    k->state = Manager;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QLabel *title = new QLabel(tr("Papagayo LipSync Files"));
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(QFont("Arial", 8, QFont::Bold));

    k->layout->addWidget(title);

    k->settingsLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    k->settingsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    k->settingsLayout->setMargin(0);
    k->settingsLayout->setSpacing(0);

    setLipSyncManagerPanel();
    setButtonsPanel();
    setPropertiesPanel();

    k->layout->addLayout(k->settingsLayout);
    k->layout->addStretch(2);
}

Configurator::~Configurator()
{
    delete k;
}

void Configurator::loadLipSyncList(QList<QString> list)
{
    k->manager->loadLipSyncList(list);
    /*
    if (list.count() > 0) {
        tError() << "Configurator::loadLipSyncList() - Tracing!";
        activeButtonsPanel(true);
    }
    */
}

void Configurator::setPropertiesPanel()
{
    k->settingsPanel = new Settings(this);

    /*
    connect(k->settingsPanel, SIGNAL(startingPointChanged(int)), this, SIGNAL(startingPointChanged(int)));
    connect(k->settingsPanel, SIGNAL(clickedSelect()), this, SIGNAL(clickedSelect()));
    connect(k->settingsPanel, SIGNAL(clickedDefineAngle()), this, SIGNAL(clickedDefineAngle()));
    connect(k->settingsPanel, SIGNAL(clickedApplyLipSync()), this, SLOT(applyItem()));
    connect(k->settingsPanel, SIGNAL(clickedResetLipSync()), this, SLOT(closeLipSyncProperties()));
    */

    k->settingsLayout->addWidget(k->settingsPanel);

    activePropertiesPanel(false);
}

void Configurator::activePropertiesPanel(bool enable)
{
    if (enable)
        k->settingsPanel->show();
    else
        k->settingsPanel->hide();
}

void Configurator::setCurrentLipSync(TupLipSync *lipsync)
{
    k->lipSync = lipsync;
}

void Configurator::setLipSyncManagerPanel()
{
    k->manager = new LipSyncManager(this);
    connect(k->manager, SIGNAL(removeCurrentLipSync(const QString &)), this, SIGNAL(removeCurrentLipSync(const QString &)));

    /*
    connect(k->manager, SIGNAL(addNewLipSync(const QString &)), this, SLOT(addLipSync(const QString &)));
    connect(k->manager, SIGNAL(editCurrentLipSync(const QString &)), this, SLOT(editLipSync()));
    connect(k->manager, SIGNAL(getLipSyncData(const QString &)), this, SLOT(updateLipSyncData(const QString &)));
    */

    k->settingsLayout->addWidget(k->manager);
    k->state = Manager;
}

void Configurator::activeLipSyncManagerPanel(bool enable)
{
    if (enable)
        k->manager->show();
    else
        k->manager->hide();

    if (k->manager->listSize() > 0) {
        tError() << "Configurator::activeLipSyncManagerPanel() - Tracing! -> " << enable;

        activeButtonsPanel(enable);
    }
}

void Configurator::setButtonsPanel()
{
    k->controlPanel = new ButtonsPanel(this);
    /*
    connect(k->controlPanel, SIGNAL(clickedEditLipSync()), this, SLOT(editLipSync()));
    connect(k->controlPanel, SIGNAL(clickedRemoveLipSync()), this, SLOT(removeLipSync()));
    */

    k->settingsLayout->addWidget(k->controlPanel);

    activeButtonsPanel(false);
}

void Configurator::activeButtonsPanel(bool enable)
{
    if (enable)
        k->controlPanel->show();
    else
        k->controlPanel->hide();
}

void Configurator::initStartCombo(int framesTotal, int currentFrame)
{
    k->framesTotal = framesTotal;
    k->currentFrame = currentFrame;
    k->settingsPanel->initStartCombo(framesTotal, currentFrame);
}

void Configurator::setStartFrame(int currentIndex)
{
    k->currentFrame = currentIndex;
    k->settingsPanel->setStartFrame(currentIndex);
}

int Configurator::startFrame()
{
    return k->settingsPanel->startFrame();
}

int Configurator::startComboSize()
{
    return k->settingsPanel->startComboSize();
}

QString Configurator::lipSyncToXml(int currentScene, int currentLayer, int currentFrame, QPointF point)
{
    return k->settingsPanel->lipSyncToXml(currentScene, currentLayer, currentFrame, point);
}

int Configurator::totalSteps()
{
    return k->settingsPanel->totalSteps();
}

void Configurator::activateMode(TupToolPlugin::EditMode mode)
{
    k->settingsPanel->activateMode(mode);
}

void Configurator::addLipSync(const QString &name)
{
    k->mode = TupToolPlugin::Add;
    emit setMode(k->mode);

    activeLipSyncManagerPanel(false);

    // k->mode = TupToolPlugin::Add;
    k->state = Properties;

    k->settingsPanel->setParameters(name, k->framesTotal, k->currentFrame);
    activePropertiesPanel(true);

    // emit setMode(k->mode);
}

void Configurator::editLipSync()
{
    k->mode = TupToolPlugin::Edit;
    emit setMode(k->mode);

    activeLipSyncManagerPanel(false);

    // k->mode = TupToolPlugin::Edit;
    k->state = Properties;

    k->settingsPanel->notifySelection(true);
    k->settingsPanel->setParameters(k->lipSync);
    activePropertiesPanel(true);

    // emit setMode(k->mode);
}

QString Configurator::currentLipSyncName() const
{
/*
    QString oldName = k->manager->lipSyncName();
    QString newName = k->settingsPanel->lipSyncName();

    if (oldName.compare(newName) != 0)
        k->manager->updateLipSyncName(newName);
*/
    QString newName = "";

    return newName;
}

void Configurator::notifySelection(bool flag)
{
    k->settingsPanel->notifySelection(flag);
}

void Configurator::closeLipSyncProperties()
{
    // if (k->mode == TupToolPlugin::Add)
    //     k->manager->removeItemFromList();

    emit clickedResetInterface();

    closeSettingsPanel();
}

void Configurator::closeSettingsPanel()
{
    if (k->state == Properties) {
        activeLipSyncManagerPanel(true);
        activePropertiesPanel(false);
        k->mode = TupToolPlugin::View;
        k->state = Manager;
    } 
}

TupToolPlugin::Mode Configurator::mode()
{
    return k->mode;
}

void Configurator::applyItem()
{
     k->mode = TupToolPlugin::Edit;
     emit clickedApplyLipSync();
}

void Configurator::resetUI()
{
    k->manager->resetUI();
    closeSettingsPanel();
    k->settingsPanel->notifySelection(false);
}

void Configurator::updateLipSyncData(const QString &name)
{
    emit getLipSyncData(name);
}
