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

#ifndef TUPFRAMESTABLE_H
#define TUPFRAMESTABLE_H

#include "tglobal.h"
#include "tuptimelineruler.h"
#include "tuplayerheader.h"
#include "tupprojectactionbar.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QPainter>
#include <QHeaderView>

class TupFramesTable;
class TupFramesTableItemDelegate;

class TUPI_EXPORT TupFramesTableItem : public QTableWidgetItem
{
    public:
        enum Attributes
        {
            IsUsed = 0x0100,
            IsLocked,
            IsSound
        };
        
        TupFramesTableItem();
        virtual ~TupFramesTableItem();
        
        bool isUsed();
        bool isLocked();
        bool isSound();
};

class TupTimeLineRuler;

/**
 * @author David Cuadrado
*/
class TUPI_EXPORT TupFramesTable : public QTableWidget
{
    Q_OBJECT;
    
    friend class TupFramesTableItemDelegate;
    
    public:
        TupFramesTable(int sceneIndex = 0, QWidget *parent = 0);
        ~TupFramesTable();
        
        bool isSoundLayer(int row);
        void setLayerVisibility(int layerIndex, bool isVisible);
        void setLayerName(int layerIndex, const QString &name);
        
    public slots:
        // Layers
        void insertLayer(int index, const QString &name);
        void insertSoundLayer(int layerPos, const QString &name);
        void removeCurrentLayer();
        void removeLayer(int pos);
        void moveLayer(int pos, int newPos);
        int lastFrameByLayer(int layerPos);
        void updateLayerHeader(int layerIndex);
        int currentLayer();
        int layersTotal();

        // Frames
        void insertFrame(int layerPos, const QString &name);
        void selectFrame(int index);
        void setAttribute(int row, int col, TupFramesTableItem::Attributes att, bool value);
        void removeFrame(int layerPos, int position);
        void lockFrame(int layerPosition, int position, bool lock);
        void setItemSize(int w, int h);
        
    private:
        void setup();
        
    protected:
        void fixSize();
        void mousePressEvent(QMouseEvent * event);
        
    private slots:
        void frameSelectionFromRuler(int frameIndex);
        void frameSelectionFromLayerHeader(int layerIndex);

        void requestFrameSelection(QTableWidgetItem *current, QTableWidgetItem *previous);
        void requestFrameSelection(int currentRow, int currentColumn, int previousRow, int previousColumn);
        
    signals:
        void frameRequest(int action, int frame, int layer, int scene, const QVariant &argument = QVariant());
        void frameHasChanged(int sceneIndex, int layerIndex, int frameIndex);
        void frameSelectionIsRequired(int layer, int frame);
        void visibilityHasChanged(int layer, bool isVisible);
        void layerNameHasChanged(int layer, const QString &name);
        
    private:
        struct Private;
        Private *const k;
};

#endif
