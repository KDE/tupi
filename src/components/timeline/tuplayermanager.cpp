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

#include "tuplayermanager.h"
#include "tapplication.h"
#include "tdebug.h"

////////////////////////////////

struct TupLayerManager::Private
{
    Private() : allSelected(false), allVisible(true), allLock(false), rowHeight(20), sceneIndex(0) {}
    
    bool allSelected, allVisible, allLock;
    int rowHeight;
    int sceneIndex;
    TupLayerIndex *layerIndex;
    TupLayerControls *layerControls;
};

TupLayerManager::TupLayerManager(int sceneIndex, QWidget *parent) : QWidget(parent), k(new Private)
{
    #ifdef K_DEBUG
        TINIT;
    #endif

    k->layerIndex = new TupLayerIndex(sceneIndex); 
    k->layerIndex->setFixedWidth(200);
    k->layerControls = new TupLayerControls(sceneIndex);
    k->layerControls->setFixedWidth(44);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 16);
    layout->setSpacing(0);
    layout->addWidget(k->layerIndex);
    layout->addWidget(k->layerControls);

    setLayout(layout);


}

TupLayerManager::~TupLayerManager()
{
    #ifdef K_DEBUG
        TEND;
    #endif

    delete k;
}

TupLayerIndex* TupLayerManager::getLayerIndex()
{
    return k->layerIndex;
} 

TupLayerControls* TupLayerManager::getLayerControls()
{
    return k->layerControls;
}

void TupLayerManager::insertLayer(int position, const QString &name)
{
    k->layerIndex->insertLayer(position, name);
    k->layerControls->insertLayer(position);
}

void TupLayerManager::insertSoundLayer(int position, const QString &name)
{
    Q_UNUSED(position);
    Q_UNUSED(name);
}

void TupLayerManager::removeLayer(int position)
{
    Q_UNUSED(position);
}

void TupLayerManager::renameLayer(int position, const QString &name)
{
    Q_UNUSED(position);
    Q_UNUSED(name);
}

void TupLayerManager::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    k->layerIndex->refresh();
}

void TupLayerManager::moveLayer(int position, int newPosition)
{
    Q_UNUSED(position);
    Q_UNUSED(newPosition);
}

void TupLayerManager::lockLayer(int position, bool locked)
{
    Q_UNUSED(position);
    Q_UNUSED(locked);
}
