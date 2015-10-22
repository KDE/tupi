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

#ifndef TUPSCENETABWIDGET_H
#define TUPSCENETABWIDGET_H

#include "tglobal.h"
#include "tupexposuretable.h"
#include "tapplicationproperties.h"

#include <QTabWidget>
#include <QList>
#include <QLabel>
#include <QFrame>
#include <QWheelEvent>
#include <QTabBar>
#include <QVBoxLayout>
#include <QDoubleSpinBox>

/**
 * @author Gustav Gonzalez 
*/

class T_GUI_EXPORT TupSceneTabWidget : public QFrame
{
    Q_OBJECT

    public:
        TupSceneTabWidget(QWidget *parent = 0);
        ~TupSceneTabWidget();
        void addScene(int index, const QString &name, TupExposureTable *table = 0);
        void removeScene(int index);
        void renameScene(int index, const QString &name);
        TupExposureTable* getCurrentTable();
        TupExposureTable* getTable(int index);
        void setCurrentIndex(int index);
        int currentIndex();
        bool isTableIndexValid(int index);
        int count();
        void setLayerOpacity(int sceneIndex, double opacity);
        void setLayerVisibility(int sceneIndex, int layerIndex, bool visibility);

    public slots:
        void removeAllTabs();

    signals:
        void currentChanged(int index);
        void updateLayerOpacity(double opacity);

    private:
        struct Private;
        Private *const k;

};

#endif
