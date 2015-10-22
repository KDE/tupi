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

#include "tglobal.h"
#include "tuptoolplugin.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QBoxLayout>
#include <QHeaderView>
#include <QGraphicsPathItem>
#include <QDir>

// class QGraphicsPathItem;
class TupItemTweener;

/**
 * @author Jorge Cuadrado
*/

class TUPI_PLUGIN Settings : public QWidget 
{
    Q_OBJECT

    public:
        Settings(QWidget *parent = 0);
        ~Settings();

        void setParameters(const QString &name, int framesCount, int startFrame);
        void setParameters(TupItemTweener *currentTween);
        void initStartCombo(int totalFrames, int currentIndex);
        void setStartFrame(int currentIndex);
        int startFrame();

        void updateSteps(const QGraphicsPathItem *path);
        QString tweenToXml(int currentScene, int currentLayer, int currentFrame, QPointF point, QString &path);
        int totalSteps();
        // void activatePathMode();
        // void activateSelectionMode();
        void activateMode(TupToolPlugin::EditMode mode);
        void cleanData();
        void notifySelection(bool flag);
        int startComboSize();
        QString currentTweenName() const;
        
    private slots:
        void emitOptionChanged(int option);
        // void addTween();
        void applyTween();
        
    signals:
        void clickedCreatePath();
        void clickedSelect();
        void clickedResetTween();
        void clickedApplyTween();
        void startingFrameChanged(int);
        
    private:
        void setInnerForm();
        void activeInnerForm(bool enable);
        void setEditMode();
        struct Private;
        Private *const k;
};

#endif
