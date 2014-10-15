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

#ifndef TUPLAYERHEADER_H
#define TUPLAYERHEADER_H

#include "tglobal.h"
#include "tapplicationproperties.h"

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QLineEdit>

struct TimeLineLayerItem
{
    QString title;
    int lastFrame;
    bool isVisible;
    bool isLocked;
};

class TUPI_EXPORT TupLayerHeader : public QHeaderView
{
    Q_OBJECT

    public:
        TupLayerHeader(QWidget * parent = 0);
        ~TupLayerHeader();
        void insertLayer(int index, const QString &name);
        void setLayerVisibility(int layerIndex, bool visibility);
        void setLayerName(int layerIndex, const QString &name);

    protected:
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;
        virtual void mousePressEvent(QMouseEvent *event);

    public slots:
        void updateSelection(int layerIndex);

    private slots:
        void showTitleEditor(int section);
        void hideTitleEditor();

    signals:
        void selectionHasChanged(int section);
        void visibilityHasChanged(int section, bool isVisible);
        void nameHasChanged(int section, const QString &name);

    private:
        struct Private;
        Private *const k;
};

#endif
