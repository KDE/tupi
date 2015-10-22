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

#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include "tglobal.h"
#include "tuptoolplugin.h"
#include "settings.h"

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QBoxLayout>
#include <QHeaderView>
#include <QGraphicsPathItem>
#include <QMenu>
#include <QAction>

// class QGraphicsPathItem;
// class QListWidgetItem;
class TupItemTweener;

/**
 * @author Jorge Cuadrado
*/

class TUPI_PLUGIN Configurator : public QFrame
{
    Q_OBJECT

    public:
        enum GuiState { Manager = 1, Properties };

        Configurator(QWidget *parent = 0);
        ~Configurator();

        void loadTweenList(QList<QString> tweenList);

        void setPropertiesPanel();
        void activePropertiesPanel(bool enable);

        void setTweenManagerPanel();
        void activeTweenManagerPanel(bool enable);

        void setButtonsPanel();
        void activeButtonsPanel(bool enable);

        void initStartCombo(int framesCount, int currentFrame);
        void setStartFrame(int currentIndex);
        int startFrame();

        void updateSteps(const QGraphicsPathItem *path);
        QString tweenToXml(int currentScene, int currentLayer, int currentFrame, QPointF point, QString &path);
        int totalSteps();
        // void activateSelectionMode();
        void activateMode(TupToolPlugin::EditMode mode);
        void cleanData();
        QString currentTweenName() const;
        void notifySelection(bool flag);
        int startComboSize();
        void closeSettingsPanel();
        TupToolPlugin::Mode mode();
        void resetUI();
        void setCurrentTween(TupItemTweener *currentTween);

    public slots:
        void closeTweenProperties();
        
    private slots:
        void applyItem();
        void addTween(const QString &name);
        void editTween();
        void removeTween();
        void removeTween(const QString &name);
        void updateTweenData(const QString &name);
        
    signals:
        void clickedCreatePath();
        void clickedSelect();
        void clickedRemoveTween(const QString &);
        void setMode(TupToolPlugin::Mode mode);
        void clickedResetInterface();
        void clickedApplyTween();
        void startingFrameChanged(int);
        // void editModeOn();
        void getTweenData(const QString &);
        
    private:
        struct Private;
        Private *const k;
};

#endif
