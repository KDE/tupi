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

#include "tuptimelinetable.h"

////////// TupTimeLineTableItemDelegate ///////////

class TupTimeLineTableItemDelegate : public QItemDelegate
{
    public:
        TupTimeLineTableItemDelegate(QObject * parent = 0);
        ~TupTimeLineTableItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

TupTimeLineTableItemDelegate::TupTimeLineTableItemDelegate(QObject * parent) : QItemDelegate(parent) // QAbstractItemDelegate(parent)
{
}

TupTimeLineTableItemDelegate::~TupTimeLineTableItemDelegate()
{
}

void TupTimeLineTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QItemDelegate::paint(painter, option, index);
    TupTimeLineTable *table = qobject_cast<TupTimeLineTable *>(index.model()->parent());
    TupTimeLineTableItem *item = dynamic_cast<TupTimeLineTableItem *>(table->itemFromIndex(index));
    
    // draw the background color
    QVariant value = index.data(Qt::BackgroundColorRole);
    
    if (value.isValid()) {
        painter->save();
        
        bool sound = table->isSoundLayer(index.row());
        if (!sound)
            painter->fillRect(option.rect, value.value<QColor>());
        
        painter->restore();
    } else {
        painter->save();
        
        bool sound = table->isSoundLayer(index.row());
        
        if (!sound) {
            int counter = index.column() + 1;
            if (counter == 1 || counter % 5 == 0) 
                painter->fillRect(option.rect, QColor(230, 230, 230));
            else 
                painter->fillRect(option.rect, Qt::white);
        } else {

        }
        
        painter->restore();
    }
    
    // Selection!
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {
        painter->save();
        painter->fillRect(option.rect, QColor(0, 136, 0, 180));
        painter->restore();
    }
    
    // Draw attributes
    int offset = option.rect.width() - 5;

    if (item && index.isValid()) {
        if (item->isUsed()) {
            painter->save();
            painter->setBrush(Qt::black);
            painter->setRenderHint(QPainter::Antialiasing, true);
            
            if (!item->isSound()) {
                if (item->isLocked()) {
                    painter->setPen(QPen(Qt::red, 1, Qt::SolidLine));
                    painter->setBrush(Qt::red);
                } 
                painter->drawEllipse(option.rect.x() + ((option.rect.width() - offset)/2), 
                                     option.rect.y() + ((option.rect.height() + offset)/2), 
                                     offset, offset);
            } else {
                painter->setBrush(QColor(0, 136, 0));
                painter->drawRect(option.rect.x() + ((option.rect.width() - offset)/2), 
                                  option.rect.y() + ((option.rect.height() + offset)/2), 
                                  offset, offset);
            }
            
            painter->restore();
        }
    }
}

////////// TupTimeLineTableItem ////////

TupTimeLineTableItem::TupTimeLineTableItem()
{
}

TupTimeLineTableItem::~TupTimeLineTableItem()
{
}

bool TupTimeLineTableItem::isUsed()
{
    return data(IsUsed).toBool();
}

bool TupTimeLineTableItem::isLocked()
{
    return data(IsLocked).toBool();
}

bool TupTimeLineTableItem::isSound()
{
    QVariant data = this->data(IsSound);
    
    if (data.canConvert<bool>())
        return data.toBool();

    return false;
}

//// TupTimeLineTable

struct TupTimeLineTable::Private
{
    /*
    struct LayerItem {
        LayerItem() : lastItem(-1), sound(false) {};
        int lastItem;
        bool sound;
    };
    */
    
    int rectWidth;
    int rectHeight;

    int sceneIndex;
    int layerIndex;
    int frameIndex;

    // QList<LayerItem> layers;
    TupTimeLineRuler *ruler;
    TupTimeLineHeader *layerColumn;

    bool removingLayer;
    bool removingFrame;
    bool isLocalRequest;
};

TupTimeLineTable::TupTimeLineTable(int sceneIndex, QWidget *parent) : QTableWidget(0, 200, parent), k(new Private)
{
    k->isLocalRequest = false;
    k->sceneIndex = sceneIndex;
    k->frameIndex = 0;
    k->layerIndex = 0;

    k->ruler = new TupTimeLineRuler;
    connect(k->ruler, SIGNAL(selectionChanged(int)), this, SLOT(frameSelectionFromRuler(int)));

    k->removingLayer = false;
    k->removingFrame = false;

    k->layerColumn = new TupTimeLineHeader;
    connect(k->layerColumn, SIGNAL(nameChanged(int, const QString &)), this, SIGNAL(layerNameChanged(int, const QString &)));
    connect(k->layerColumn, SIGNAL(selectionChanged(int)), this, SLOT(frameSelectionFromLayerHeader(int)));
    connect(k->layerColumn, SIGNAL(visibilityChanged(int, bool)), this, SIGNAL(visibilityChanged(int, bool)));
    connect(k->layerColumn, SIGNAL(sectionMoved(int, int, int)), this, SLOT(requestLayerMove(int, int, int)));

    setup();
}

TupTimeLineTable::~TupTimeLineTable()
{
    delete k;
}

void TupTimeLineTable::setup()
{
    setItemDelegate(new TupTimeLineTableItemDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(requestFrameSelection(int, int, int, int)));
    // connect(this, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(requestFrameSelection(QTableWidgetItem *, QTableWidgetItem *)));

    setHorizontalHeader(k->ruler);
    setVerticalHeader(k->layerColumn);

    setItemSize(10, 25);
    
    // horizontalHeader()->setSectionResizeMode(QHeaderView::Custom);
    k->ruler->setSectionResizeMode(QHeaderView::Custom);
    k->layerColumn->setSectionResizeMode(QHeaderView::Custom);
}

void TupTimeLineTable::frameSelectionFromRuler(int frameIndex)
{
    emit frameSelected(0, frameIndex);
}

void TupTimeLineTable::frameSelectionFromLayerHeader(int layerIndex)
{
    tError() << "TupTimeLineTable::frameSelectionFromLayerHeader() - layerIndex -> " << layerIndex;

    if (k->layerColumn->currentSectionIndex() != layerIndex)
        k->layerColumn->updateSelection(layerIndex);

    emit frameSelected(layerIndex, currentColumn());
}

void TupTimeLineTable::requestLayerMove(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    if (!k->layerColumn->sectionIsMoving()) {
        int distance = newVisualIndex - oldVisualIndex;
        if (distance > 0) {
            if (distance > 1)
                newVisualIndex = oldVisualIndex + 1;
        } else {
            distance = abs(distance);
            if (distance > 1)
                newVisualIndex = oldVisualIndex - 1;
        }

        k->isLocalRequest = true;
        emit layerMoved(oldVisualIndex, newVisualIndex);
    }
}

/*
void TupTimeLineTable::requestFrameSelection(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);

    TupTimeLineTableItem *item = dynamic_cast<TupTimeLineTableItem *>(current);
    
    if (item) {
        if (item->isUsed()) {
            tError() << "TupTimeLineTable::requestFrameSelection() - Just tracing 1";
            emit frameChanged(k->sceneIndex, verticalHeader()->visualIndex(this->row(item)), this->column(item));
        } else {
            #ifdef K_DEBUG
                QString msg = "TupTimeLineTable::requestFrameSelection <- item exists but is unset right now";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal() << msg;
                #endif
            #endif  
        }
    } else { 
        tError() << "TupTimeLineTable::requestFrameSelection() - Called from -> currentItemChanged()";
        tError() << "";
        emit frameRequest(TupProjectActionBar::InsertFrame, currentColumn(), currentRow(), k->sceneIndex);
    }
}
*/

void TupTimeLineTable::setItemSize(int w, int h)
{
    k->rectHeight = h;
    k->rectWidth = w;
    
    fixSize();
}

bool TupTimeLineTable::isSoundLayer(int index)
{
    // if (index < 0 && index >= k->layers.count())
    if (index < 0 && index >= rowCount())
        return false;
    
    // return k->layers[index].sound;

    return k->layerColumn->isSound(index);
}

void TupTimeLineTable::insertLayer(int index, const QString &name)
{
    tError() << "TupTimeLineTable::insertLayer() - Inserting layer at index: " << index;

    insertRow(index);

    /*    
    Private::LayerItem layer;
    layer.sound = false;
    k->layers.insert(index, layer);
    */

    k->layerColumn->insertSection(index, name);
    
    fixSize();
}

void TupTimeLineTable::insertSoundLayer(int index, const QString &name)
{
    insertRow(index);

    /*
    Private::LayerItem layer;
    layer.sound = true;
    k->layers.insert(index, layer);
    */

    k->layerColumn->insertSection(index, name);
    
    fixSize();
}

/*
void TupTimeLineTable::removeCurrentLayer()
{
    // int pos = verticalHeader()->logicalIndex(currentRow());
    // removeLayer(pos);

    removeLayer(currentRow());
}
*/

void TupTimeLineTable::removeLayer(int index)
{
    tError() << "TupTimeLineTable::removeLayer() - Removing layer at: " << index;
    tError() << "TupTimeLineTable::removeLayer() - rowCount(): " << rowCount();

    // setUpdatesEnabled(false);
    k->removingLayer = true;

    removeRow(index);
    k->layerColumn->removeSection(index);

    // setUpdatesEnabled(true);

    /*
    if (rowCount() > 1) {
        removeRow(index);
        k->layerColumn->removeSection(index);
    } else {
        for(int i=0; i<= k->layerColumn->lastFrame(0); i++) 
            setAttribute(0, i, TupTimeLineTableItem::IsUsed, false);

        k->layerColumn->resetLastFrame(0);
    }
    */
}

void TupTimeLineTable::moveLayer(int index, int newIndex)
{
    if (index < 0 || index >= rowCount() || newIndex < 0 || newIndex >= rowCount()) 
        return;

    k->layerColumn->moveHeaderSection(index, newIndex, k->isLocalRequest);
    if (k->isLocalRequest)
        k->isLocalRequest = false;

    for (int frameIndex = 0; frameIndex <= k->layerColumn->lastFrame(index); frameIndex++)
         exchangeFrame(frameIndex, index, newIndex);

    blockSignals(true); 
    selectFrame(newIndex, currentColumn());
    blockSignals(false);
}

void TupTimeLineTable::exchangeFrame(int frameIndex, int currentLayer, int newLayer)
{
    QTableWidgetItem *oldItem = takeItem(currentLayer, frameIndex);
    QTableWidgetItem *newItem = takeItem(newLayer, frameIndex);

    setItem(newLayer, frameIndex, oldItem);
    setItem(currentLayer, frameIndex, newItem);
}

void TupTimeLineTable::setLayerVisibility(int layerIndex, bool isVisible)
{
    k->layerColumn->setSectionVisibility(layerIndex, isVisible);
}

void TupTimeLineTable::setLayerName(int layerIndex, const QString &name)
{
    k->layerColumn->setSectionTitle(layerIndex, name);
}

int TupTimeLineTable::currentLayer()
{
    return currentRow();
}

int TupTimeLineTable::layersTotal()
{
    return rowCount();
}

int TupTimeLineTable::lastFrameByLayer(int index)
{
    if (index < 0 || index >= rowCount())
        return -1;

    return k->layerColumn->lastFrame(index);
}

// FRAMES

void TupTimeLineTable::insertFrame(int layerIndex, const QString &name)
{
    Q_UNUSED(name);

    tError() << "TupTimeLineTable::insertFrame() - Inserting frame at layer index -> " << layerIndex;

    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    k->layerColumn->updateLastFrame(layerIndex, true);
  
    int lastFrame = k->layerColumn->lastFrame(layerIndex); 

    setAttribute(layerIndex, lastFrame, TupTimeLineTableItem::IsUsed, true);
    setAttribute(layerIndex, lastFrame, TupTimeLineTableItem::IsSound, false);

    // viewport()->update();
}

/*
void TupTimeLineTable::setCurrentFrame(TupTimeLineTableItem *item)
{
    setCurrentItem(item);
}
*/

void TupTimeLineTable::updateLayerHeader(int layerIndex)
{
    k->layerColumn->updateSelection(layerIndex); 
}

void TupTimeLineTable::selectFrame(int index)
{
    setCurrentItem(item(currentRow(), index));
}

void TupTimeLineTable::removeFrame(int index, int position)
{
    Q_UNUSED(position);

    tError() << "TupTimeLineTable::removeFrame() - Removing frame...";

    if (index < 0 || index >= rowCount())
        return;

    k->removingFrame = true;
    
    setAttribute(index, k->layerColumn->lastFrame(index), TupTimeLineTableItem::IsUsed, false);

    k->layerColumn->updateLastFrame(index, false);
    viewport()->update();
}

void TupTimeLineTable::lockFrame(int index, int position, bool lock)
{
    // if (index < 0 || index >= k->layers.count())

    if (index < 0 || index >= rowCount())
        return;
    
    // index = verticalHeader()->logicalIndex(index);

    setAttribute(index, position, TupTimeLineTableItem::IsLocked, lock);
    viewport()->update();
}

void TupTimeLineTable::setAttribute(int row, int col, TupTimeLineTableItem::Attributes att, bool value)
{
    QTableWidgetItem *item = this->item(row, col);
    
    if (!item) {
        item = new TupTimeLineTableItem;
        setItem(row, col, item);
    }
    
    item->setData(att, value);
}

void TupTimeLineTable::fixSize()
{
    for (int column = 0; column < columnCount(); column++)
         horizontalHeader()->resizeSection(column, k->rectWidth);

    for (int row = 0; row < rowCount(); row++)
         k->layerColumn->resizeSection(row, k->rectHeight);
         // verticalHeader()->resizeSection(row, k->rectHeight);
}

void TupTimeLineTable::requestFrameSelection(int currentSelectedRow, int currentSelectedColumn, int previousRow, int previousColumn)
{
    tError() << "TupTimeLineTable::requestFrameSelection() - Tracing... * * * * ";

    if (!k->removingLayer) {
        if (k->removingFrame) {
            tError() << "TupTimeLineTable::requestFrameSelection() - A frame is being removed...";
            k->removingFrame = false;
            emit frameSelected(currentRow(), currentColumn());
            return;
        } else {
            if (previousColumn != currentSelectedColumn || previousRow != currentSelectedRow) 
                emit frameSelected(currentRow(), currentColumn());
        }
    } else { // A layer is being removed
        k->removingLayer = false;

        if (previousRow != 0) {
            if (previousRow != rowCount() - 1) {
                blockSignals(true);
                setCurrentItem(item(previousRow - 1, currentColumn())); 
                k->layerColumn->updateSelection(previousRow - 1);
                blockSignals(false);
            }
        }
    }
}

void TupTimeLineTable::mousePressEvent(QMouseEvent *event)
{
    int frameIndex = columnAt(event->x());

    int total = columnCount();
    if ((frameIndex >= total - 11) && (frameIndex <= total - 1)) {
        int newTotal = total + 100;
        for (int i=total; i < newTotal; i++) {
             insertColumn(i);
        }
        fixSize();
    }

    QTableWidget::mousePressEvent(event);
}

void TupTimeLineTable::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) {
        int limit = columnCount()-1;
        int next = currentColumn()+1;
        if (next <= limit) 
            setCurrentCell(currentRow(), next);
    }    

    if (event->key() == Qt::Key_Left) {
        int next = currentColumn()-1;
        if (next >= 0) 
            setCurrentCell(currentRow(), next);
    }

    if (event->key() == Qt::Key_Up) {
        int next = currentRow()-1;
        if (next >= 0) 
            setCurrentCell(next, currentColumn());
    }

    if (event->key() == Qt::Key_Down) {
        int limit = rowCount()-1;
        int next = currentRow()+1;
        if (next <= limit)
            setCurrentCell(next, currentColumn());
    }
}

void TupTimeLineTable::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QTableWidget::enterEvent(event);
}

void TupTimeLineTable::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QTableWidget::leaveEvent(event);
}

void TupTimeLineTable::selectFrame(int layerIndex, int frameIndex)
{
    blockSignals(true);
    setCurrentCell(layerIndex, frameIndex);
    updateLayerHeader(layerIndex);
    tError() << "TupTimeLineTable::selectFrame() - lastFrame(int index): " << k->layerColumn->lastFrame(layerIndex);
    blockSignals(false);
}
