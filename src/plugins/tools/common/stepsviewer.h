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

#ifndef STEPSVIEWER_H
#define STEPSVIEWER_H

#include "tglobal.h"
#include "tuptweenerstep.h"
#include "spinboxdelegate.h"
#include "tpushbutton.h"

#include <QTableWidget>
#include <cmath>
#include <QGraphicsPathItem>
#include <QDebug>
#include <QBoxLayout>
#include <QHeaderView>
#include <QPainter>

// class QGraphicsPathItem;
// class TupTweenerStep;

/**
 * @author Jorge Cuadrado 
*/

class TUPI_EXPORT StepsViewer : public QTableWidget
{
    Q_OBJECT

    // friend class TupExposureVerticalHeader;

    public:
        StepsViewer(QWidget *parent = 0);
        ~StepsViewer();
        void setPath(const QGraphicsPathItem *path);
        
        QVector<TupTweenerStep *> steps();
        int totalSteps();
        void cleanRows();
        virtual QSize sizeHint() const;

    private slots:
        void updatePath(int column, int row);
        
    private:
        QList<QPointF> calculateDots(QPointF dot1, QPointF dot2, int total);
        struct Private;
        Private *const k;

    signals:
        void updateTable();
};

#endif
