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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "tuptoolplugin.h"
#include <QWidget>

class TupItemTweener;

/**
 * @author Gustav Gonzalez 
*/

class Settings : public QWidget 
{
    Q_OBJECT

    public:
        Settings(QWidget *parent = 0);
        ~Settings();

        void setParameters(const QString &name, int framesTotal, int startFrame);
        void setParameters(TupItemTweener *currentTween);
        void initStartCombo(int totalFrames, int currentIndex);
        void setStartFrame(int currentIndex);
        int startFrame();

        int totalSteps();

        void notifySelection(bool flag);
        int startComboSize();
        QString currentTweenName() const;
        void activateMode(TupToolPlugin::EditMode mode);
        QString tweenToXml(int currentScene, int currentLayer, int currentFrame, QPointF point);

    private slots:
        void applyTween();
        void emitOptionChanged(int option);
        void refreshForm(int type);
        void checkTopLimit(int index);
        void updateRangeCheckbox(int state);
        void updateReverseCheckbox(int state);
        void updateTotalSteps(const QString &text);
        void checkRange(int index);
        void updateLastFrame();
        
    signals:
        void clickedSelect();
        void clickedDefineAngle();
        void clickedApplyTween();
        void clickedResetTween();
        void startingPointChanged(int index);
        
    private:
        void setInnerForm();
        void activeInnerForm(bool enable);
        void setClockForm();
        void activeClockForm(bool enable); 
        void setRangeForm();
        void activeRangeForm(bool enable);
        void setEditMode();
        void checkFramesRange();

        struct Private;
        Private *const k;
};

#endif
