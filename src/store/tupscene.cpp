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

#include "tupscene.h"
#include "tupgraphicobject.h"
#include "tupsvgitem.h"
#include "tupsoundlayer.h"
#include "tupitemgroup.h"
#include "tupprojectloader.h"
#include "tupitemfactory.h"

struct TupScene::Private
{
    QSize dimension;
    QColor bgColor;
    TupStoryboard *storyboard;
    TupBackground *background;
    Layers layers;
    SoundLayers soundLayers;
    QString name;
    bool isLocked;
    int layerCount;
    bool isVisible;

    QList<TupGraphicObject *> tweeningGraphicObjects;
    QList<TupSvgItem *> tweeningSvgObjects;
};

TupScene::TupScene(TupProject *parent, const QSize dimension, const QColor bgColor) : QObject(parent), k(new Private)
{
    k->dimension = dimension;
    k->bgColor = bgColor;
    k->isLocked = false;
    k->layerCount = 0;
    k->isVisible = true;
    k->storyboard = new TupStoryboard(parent->author());
    k->background = new TupBackground(this, dimension, bgColor);
}

TupScene::~TupScene()
{
    delete k;
}

void TupScene::setSceneName(const QString &name)
{
    k->name = name;
}

void TupScene::setBgColor(const QColor bgColor)
{
    k->bgColor = bgColor;
    k->background->setBgColor(bgColor);
}

void TupScene::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

QString TupScene::sceneName() const
{
    return k->name;
}

bool TupScene::isLocked() const
{
    return k->isLocked;
}

void TupScene::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
}

bool TupScene::isVisible() const
{
    return k->isVisible;
}

Layers TupScene::layers() const
{
    return k->layers;
}

int TupScene::layersCount() const
{
    return k->layerCount;
}

SoundLayers TupScene::soundLayers() const
{
    return k->soundLayers;
}

/**
 * Set the layers list, this function overwrites the old layers
 */
void TupScene::setLayers(const Layers &layers)
{
    k->layers = layers;
}

TupLayer *TupScene::createLayer(QString name, int position, bool loaded)
{
    if (position < 0 || position > k->layers.count()) {        
        #ifdef K_DEBUG
            QString msg = "TupScene::createLayer() - Invalid index -> " + QString::number(position);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif    
        
        return 0;
    }

    k->layerCount++;
    TupLayer *layer = new TupLayer(this, k->layerCount);
    layer->setLayerName(name);
    k->layers.insert(position, layer);

    if (loaded)
        TupProjectLoader::createLayer(project()->visualIndexOf(this), position, layer->layerName(), project());

    return layer;
}

TupSoundLayer *TupScene::createSoundLayer(int position, bool loaded)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[createSoundLayer()] - position: " << position;
        #else
            T_FUNCINFO << position;
        #endif
    #endif    
    
    if (position < 0 || position > k->soundLayers.count()) {    
        #ifdef K_DEBUG
            QString msg = "TupScene::createSoundLayer() - [ Fatal Error ] - Index incorrect!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif        
        return 0;
    }

    TupSoundLayer *layer = new TupSoundLayer(this);
    k->layerCount++;

    layer->setLayerName(tr("Sound layer %1").arg(k->layerCount));

    k->soundLayers.insert(position, layer);

    if (loaded)
        TupProjectLoader::createSoundLayer(objectIndex(), position, layer->layerName(), project());

    return layer;
}

bool TupScene::removeLayer(int position)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupScene::removeLayer()] - position: " << position;
        #else
            T_FUNCINFO << position;
        #endif
    #endif

    TupLayer *layer = this->layer(position);
    if (layer) {
        removeTweensFromLayer(position + 1);
        k->layers.removeAt(position);
        k->layerCount--;
        delete layer;

        return true;
    }

    return false;
}

/**
 * Retorna el layer que se encuentra en la posicion indicada
 * @param position 
 * @return 
 */
TupLayer *TupScene::layer(int position) const
{
    //if (position < 0 || position >= k->layers.count()) {

    if (position < 0) {    
        #ifdef K_DEBUG
            QString msg1 = " FATAL ERROR: LAYERS TOTAL: " + QString::number(k->layers.count());
            QString msg2 = " FATAL ERROR: index out of bound -> Position: " + QString::number(position);
            QString msg3 = " FATAL ERROR: The layer requested doesn't exist anymore";
            #ifdef Q_OS_WIN32
                qDebug() << msg1;
                qDebug() << msg2;
                qDebug() << msg3;
            #else
                tError() << msg1;
                tError() << msg2;
                tError() << msg3;
            #endif
        #endif
        return 0;
    }

    return k->layers.value(position);
}

TupSoundLayer *TupScene::soundLayer(int position) const
{
    if (position < 0 || position >= k->soundLayers.count()) {
        #ifdef K_DEBUG
            QString msg = " FATAL ERROR: index out of bound " + QString::number(position);
            #ifdef Q_OS_WIN32
               qDebug() << msg;
            #else
               T_FUNCINFO << msg;
            #endif
        #endif
        return 0;
    }

    return k->soundLayers.value(position);
}

void TupScene::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();

    setSceneName(root.attribute("name", sceneName()));

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {
               if (e.tagName() == "layer") {
                   int pos = k->layers.count();
                   TupLayer *layer = createLayer(e.attribute("name"), pos, true);

                   if (layer) {
                       QString newDoc;
                       {
                         QTextStream ts(&newDoc);
                         ts << n;
                       }
                       layer->fromXml(newDoc);
                   }
               } else if (e.tagName() == "background") {
                          QString newDoc;
                          {
                            QTextStream ts(&newDoc);
                            ts << n;
                          }
                          k->background->fromXml(newDoc); 

               } else if (e.tagName() == "soundlayer") {
                          int pos = k->soundLayers.count();
                          TupSoundLayer *layer = createSoundLayer(pos, true);

                          if (layer) {
                              QString newDoc;
                               {
                                 QTextStream ts(&newDoc);
                                 ts << n;
                               }
                              layer->fromXml(newDoc);
                          }
               } else if (e.tagName() == "storyboard") {
                          QString newDoc;
                          {
                            QTextStream ts(&newDoc);
                            ts << n;
                          }
                          k->storyboard->fromXml(newDoc);
               }
           }

           n = n.nextSibling();

    } // end while
}

QDomElement TupScene::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("scene");
    root.setAttribute("name", k->name);

    root.appendChild(k->storyboard->toXml(doc));
    root.appendChild(k->background->toXml(doc));

    int total = k->layers.size();
    for (int i = 0; i < total; ++i) {
         TupLayer *layer = k->layers.at(i);
         root.appendChild(layer->toXml(doc));
    }

    total = k->soundLayers.size();
    for (int i = 0; i < total; ++i) {
         TupSoundLayer *sound  = k->soundLayers.at(i);
         root.appendChild(sound->toXml(doc));
    }

    return root;
}

bool TupScene::moveLayer(int from, int to)
{
    if (from < 0 || from >= k->layers.count() || to < 0 || to >= k->layers.count()) {
        #ifdef K_DEBUG
            QString msg = "TupScene::moveLayer() - FATAL ERROR: Layer index out of bound " + QString::number(to);
            #ifdef Q_OS_WIN32
                 qDebug() << msg;
            #else
                 tError() << msg;
            #endif
        #endif
        return false;
    }

    TupLayer *sourceLayer = k->layers[from];
    sourceLayer->updateLayerIndex(to + 1);
    TupLayer *destinyLayer = k->layers[to];
    destinyLayer->updateLayerIndex(from + 1); 

    Frames frames = sourceLayer->frames(); 
    int totalFrames = frames.size();
    int zLevelIndex = (to + 2)*10000;
    for (int i = 0; i < totalFrames; i++) {
         TupFrame *frame = frames.at(i);
         frame->updateZLevel(zLevelIndex);
    }

    frames = destinyLayer->frames(); 
    totalFrames = frames.size();
    zLevelIndex = (from + 2)*10000;
    for (int i = 0; i < totalFrames; i++) {
         TupFrame *frame = frames.at(i);
         frame->updateZLevel(zLevelIndex);
    }

    k->layers.swap(from, to);

    return true;
}

int TupScene::objectIndex() const
{
    if (TupProject *project = dynamic_cast<TupProject *>(parent()))
        return project->visualIndexOf(const_cast<TupScene *>(this));

    return -1;
}

int TupScene::visualIndexOf(TupLayer *layer) const
{
    return k->layers.indexOf(layer);
}

TupProject *TupScene::project() const
{
    return static_cast<TupProject *>(parent());
}

void TupScene::addTweenObject(TupGraphicObject *object)
{
    k->tweeningGraphicObjects << object;
}

void TupScene::addTweenObject(TupSvgItem *object)
{
    k->tweeningSvgObjects << object;
}

void TupScene::updateTweenObject(int index, TupGraphicObject *object)
{
    k->tweeningGraphicObjects.replace(index, object); 
}

void TupScene::updateTweenObject(int index, TupSvgItem *object)
{
    k->tweeningSvgObjects.replace(index, object);
}

void TupScene::removeTweenObject(TupGraphicObject *object)
{
    if (k->tweeningGraphicObjects.size() > 0)
        k->tweeningGraphicObjects.removeAll(object);
}

void TupScene::removeTweenObject(TupSvgItem *object)
{
    if (k->tweeningSvgObjects.size() > 0)
        k->tweeningSvgObjects.removeAll(object);
}

QList<TupGraphicObject *> TupScene::tweeningGraphicObjects() const
{
    return k->tweeningGraphicObjects;
}

QList<TupSvgItem *> TupScene::tweeningSvgObjects() const
{
    return k->tweeningSvgObjects;
}

bool TupScene::tweenExists(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type))
                     return true;
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type))
                     return true;
             }
    }

    return false;
}

bool TupScene::removeTween(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     object->removeTween();
                     removeTweenObject(object);
                     return true;
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     object->removeTween();
                     removeTweenObject(object);
                     return true;
                 }
             }
    }

    return false;
}

void TupScene::removeTweensFromLayer(int layer)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->frame()->layer()->layerIndex() == layer) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->frame()->layer()->layerIndex() == layer) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }
}

void TupScene::removeTweensFromFrame(int frame)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->frame()->index() == frame) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->frame()->index() == frame) {
                 object->removeTween();
                 removeTweenObject(object);
             }
    }
}

TupItemTweener *TupScene::tween(const QString &name, TupItemTweener::Type type)
{
    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     return tween;
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     return tween;
                 }
             }
    }

    return 0;
}

QList<QString> TupScene::getTweenNames(TupItemTweener::Type type)
{
    QList<QString> names;

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if (tween->type() == type && !names.contains(tween->name()))
                     names.append(tween->name());
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if (tween->type() == type && !names.contains(tween->name()))
                     names.append(tween->name());
             } 
    }

    return names;
}

QList<QGraphicsItem *> TupScene::getItemsFromTween(const QString &name, TupItemTweener::Type type)
{
    QList<QGraphicsItem *> items;

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     items.append(object->item());
                 }
             }
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (TupItemTweener *tween = object->tween()) {
                 if ((tween->name().compare(name) == 0) && (tween->type() == type)) {
                     items.append(object);
                 }
             }
    }

    return items;
}

int TupScene::getTotalTweens()
{
    int total = 0;

    foreach (TupGraphicObject *object, k->tweeningGraphicObjects) {
             if (object->hasTween()) 
                 total++;
    }

    foreach (TupSvgItem *object, k->tweeningSvgObjects) {
             if (object->hasTween())                    
                 total++;
    }

    return total;
}

int TupScene::framesCount()
{
    int total = 0;
    foreach (TupLayer *layer, k->layers) {
             int frames = layer->framesCount();
             if (frames > total)
                 total = frames;
    }

    return total;
}

TupBackground* TupScene::background()
{
    return k->background;
}

void TupScene::reset(QString &name)
{
    k->name = name;

    k->background = new TupBackground(this, k->dimension, k->bgColor);
    k->layers.clear();
    k->tweeningGraphicObjects.clear();
    k->tweeningSvgObjects.clear();

    k->layerCount = 1;
    TupLayer *layer = new TupLayer(this, k->layerCount);
    layer->setLayerName(tr("Layer %1").arg(1));
    layer->createFrame(tr("Frame %1").arg(1), 0, false);

    k->layers.insert(0, layer);
}

void TupScene::setStoryboard(TupStoryboard *storyboard)
{
    k->storyboard = storyboard;
}

TupStoryboard * TupScene::storyboard()
{
    return k->storyboard;
}

void TupScene::insertStoryBoardScene(int index)
{
    k->storyboard->insertScene(index);
}

void TupScene::appendStoryBoardScene()
{
    k->storyboard->appendScene();
}

void TupScene::moveStoryBoardScene(int oldIndex, int newIndex)
{
    k->storyboard->moveScene(oldIndex, newIndex);
}

void TupScene::resetStoryBoardScene(int index)
{
    k->storyboard->resetScene(index);
}

void TupScene::removeStoryBoardScene(int index)
{
    k->storyboard->removeScene(index);
}

QList<QString> TupScene::getLipSyncNames()
{
    QList<QString> names;

    foreach (TupLayer *layer, k->layers) {
             if (layer->lipSyncCount() > 0) {
                 Mouths mouths = layer->lipSyncList();
                 foreach (TupLipSync *lipsync, mouths)
                          names << lipsync->name();
             }
    }

    return names;
}


bool TupScene::lipSyncExists(const QString &name)
{
    foreach (TupLayer *layer, k->layers) {
             if (layer->lipSyncCount() > 0) {
                 Mouths mouths = layer->lipSyncList();
                 foreach (TupLipSync *lipsync, mouths) {
                          if (lipsync->name().compare(name) == 0)
                              return true;
                 }
             }
    }

    return false;
}

int TupScene::getLipSyncLayerIndex(const QString &name)
{
    int index = 0;
    foreach (TupLayer *layer, k->layers) {
             if (layer->lipSyncCount() > 0) {
                 Mouths mouths = layer->lipSyncList();
                 foreach (TupLipSync *lipsync, mouths) {
                          if (lipsync->name().compare(name) == 0)
                              break;
                          index++;
                 }
             }
    }

    return index;
}

TupLipSync * TupScene::getLipSync(const QString &name)
{
    TupLipSync *project = 0;

    foreach (TupLayer *layer, k->layers) {
             if (layer->lipSyncCount() > 0) {
                 Mouths mouths = layer->lipSyncList();
                 foreach (TupLipSync *lipsync, mouths) {
                          if (lipsync->name().compare(name) == 0) {
                              return lipsync;
                          }
                 }
             }
    }

    return project;
}

bool TupScene::updateLipSync(TupLipSync *lipsync)
{
    QString name = lipsync->name();

    foreach (TupLayer *layer, k->layers) {
             if (layer->lipSyncCount() > 0) {
                 Mouths mouths = layer->lipSyncList();
                 foreach (TupLipSync *record, mouths) {
                          if (record->name().compare(name) == 0) {
                              record = lipsync;
                              return true;
                          }
                 }
             }
    }

    return false;
}

bool TupScene::removeLipSync(const QString &name)
{
    foreach (TupLayer *layer, k->layers) {
             if (layer->removeLipSync(name))
                 return true;
    }

    return false;
}

int TupScene::lipSyncTotal()
{
    int total = 0;
    foreach (TupLayer *layer, k->layers)
             total += layer->lipSyncCount();
       
    return total;
}

Mouths TupScene::getLipSyncList()
{
    Mouths list;

    foreach (TupLayer *layer, k->layers) {
             if (layer->lipSyncCount() > 0) {
                 Mouths mouths = layer->lipSyncList();
                 list.append(mouths);
             }
    }

    return list;
}

