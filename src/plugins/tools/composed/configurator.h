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
#include "tweenerpanel.h"

#include <QFrame>
#include <QLabel>
#include <QBoxLayout>

class QGraphicsPathItem;
class QListWidgetItem;
class TupItemTweener;
// class TweenerPanel;

/**
 * @author Gustav Gonzalez 
*/

class TUPI_PLUGIN Configurator : public QFrame
{
    Q_OBJECT

    public:
        enum GuiState { Manager = 1, TweenSettings };

        Configurator(QWidget *parent = 0);
        ~Configurator();

        void loadTweenList(QList<QString> tweenList);

        void initStartCombo(int framesCount, int currentFrame);
        void setStartFrame(int currentIndex);

        void updateSteps(const QGraphicsPathItem *path, QPointF offset);

        int totalSteps();
        void activateMode(TweenerPanel::EditMode mode);

        void cleanTweensForms();
        void setCurrentTween(TupItemTweener *currentTween);
        QString currentTweenName() const;
        void notifySelection(bool flag);
        int startComboSize();
        void closeTweenerPanel();
        TweenerPanel::Mode mode();
        void resetUI();
        QString tweenToXml(int currentFrame, QPointF point);
        
    private slots:
        void applyItem();
        void addTween(const QString &name);
        void editTween();
        void removeTween();
        void removeTween(const QString &name);
        void closeTweenList();
        void updateTweenData(const QString &name);
        
    signals:
        void clickedSelect();
        void clickedTweenProperties();
        void tweenPropertiesActivated(TweenerPanel::TweenerType flag);
        void setMode(TweenerPanel::Mode mode);
        void clickedRemoveTween(const QString &name);
        void clickedApplyTween();
        void clickedResetInterface();
        void loadPath(bool enable, bool reset);
        void getTweenData(const QString &name);
        void startingPointChanged(int index);

    private:
        void setTweenerPanel();
        void activeTweenerPanel(bool enable);
        void setTweenManagerPanel();
        void activeTweenManagerPanel(bool enable);
        void setButtonsPanel();
        void activeButtonsPanel(bool enable);

        struct Private;
        Private *const k;
};

#endif
