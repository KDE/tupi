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

#include "tuplayer.h"
#include "tupscene.h"
#include "tdebug.h"
#include "tupprojectloader.h"

struct TupLayer::Private
{
    Frames frames;
    bool isVisible;
    QString name;
    int framesCount;
    bool isLocked;
    int index;
    //int zLevelBase;
};

TupLayer::TupLayer(TupScene *parent, int index) : QObject(parent), k(new Private)
{
    k->index = index;
    k->isVisible = true;
    k->name = tr("Layer");
    k->framesCount = 0;
    k->isLocked = false;
}

TupLayer::~TupLayer()
{
    k->frames.clear(true);
    delete k;
}

Frames TupLayer::frames()
{
   /*
    tFatal() << "";
    tFatal() << "TupLayer::frames - LAYER NAME: " << k->name;
    tFatal() << "";
    */

    return k->frames;
}

void TupLayer::setFrames(const Frames &frames)
{
    k->frames = frames;
    k->framesCount = frames.count();
}

void TupLayer::setFrame(int index, TupFrame *frame)
{
    k->frames.insert(index, frame);
}

void TupLayer::setLayerName(const QString &name)
{
    k->name = name;
}

void TupLayer::setLocked(bool isLocked)
{
    k->isLocked = isLocked;
}

bool TupLayer::isLocked() const
{
    return k->isLocked;
}

void TupLayer::setVisible(bool isVisible)
{
    k->isVisible = isVisible;
    // emit visibilityChanged(isVisible);
}

QString TupLayer::layerName() const
{
    return k->name;
}

bool TupLayer::isVisible() const
{
    return k->isVisible;
}

TupFrame *TupLayer::createFrame(QString name, int position, bool loaded)
{
    /*
    if (position < 0 || position > k->frames.count()) {
        tFatal() << "TupLayer::createFrame -> index is out of range: " << position << " - frames.count(): " << k->frames.count();
        return 0;
    }
    */

    if (position < 0)
        return 0;

    TupFrame *frame = new TupFrame(this);
    k->framesCount++;
    frame->setFrameName(name);
    k->frames.insert(position, frame);

    if (loaded)
        TupProjectLoader::createFrame(scene()->objectIndex(), objectIndex(), position, frame->frameName(), project());

    return frame;
}

bool TupLayer::removeFrame(int position)
{
    TupFrame *toRemove = frame(position);

    if (toRemove) {
        k->frames.removeObject(position);
        toRemove->setRepeat(toRemove->repeat()-1);

        /*
        if (toRemove->repeat() < 1) {
            tFatal() << "TupLayer::removeFrame -> Deleting pointer!";
            delete toRemove;
        }
        */

        k->framesCount--;

        return true;
    }

    return false;
}

bool TupLayer::resetFrame(int position)
{
    TupFrame *toReset = frame(position);

    if (toReset) {
        QString label = toReset->frameName();
        TupFrame *frame = new TupFrame(this); 
        frame->setFrameName(label);
        k->frames.insert(position, frame);
        return true;
    }

    return false;
}

bool TupLayer::moveFrame(int from, int to)
{
    if (from < 0 || from >= k->frames.count() || to < 0 || to > k->frames.count())
        return false;
 
    TupFrame *origin = k->frames.value(from);
    if (origin) {
        QString label = origin->frameName();
        TupFrame *frame = new TupFrame(this);
        frame->setFrameName(label);

        TupFrame *target = k->frames.value(to);
        if (target) {
            QString targetLabel = target->frameName();
            origin->setFrameName(targetLabel);
            k->frames.insert(to, origin); 
            k->frames.insert(from, frame); 
            return true;
        }
    }
    
    return false;
}

bool TupLayer::exchangeFrame(int from, int to)
{
    if (from < 0 || from >= k->frames.count() || to < 0 || to > k->frames.count())
        return false;

    k->frames.exchangeObject(from, to);

    return true;
}

bool TupLayer::expandFrame(int position, int size)
{
    if (position < 0 || position >= k->frames.count())
        return false;

    TupFrame *toExpand = frame(position);

    if (toExpand) {
        k->frames.expandValue(position, size);
        return true;
    }

    return false;
}


TupFrame *TupLayer::frame(int position) const
{
    if (position < 0 || position >= k->frames.count()) {
        #ifdef K_DEBUG
               tError() << "TupLayer::frame() - FATAL ERROR: frame index out of bound : " << position;
               tError() << "TupLayer::frame() - FATAL ERROR: index limit : " << k->frames.count()-1;
        #endif
        return 0;
    }

    return k->frames.value(position);
}


void TupLayer::fromXml(const QString &xml)
{
    QDomDocument document;

    if (! document.setContent(xml))
        return;

    QDomElement root = document.documentElement();

    setLayerName(root.attribute("name", layerName()));

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {
               if (e.tagName() == "frame") {
                   TupFrame *frame = createFrame(e.attribute("name"), k->frames.count(), true);

                   if (frame) {
                       QString newDoc;

                       {
                         QTextStream ts(&newDoc);
                         ts << n;
                       }

                       frame->fromXml(newDoc);
                   }
               }
           }

           n = n.nextSibling();
    }
}

QDomElement TupLayer::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("layer");
    root.setAttribute("name", k->name);
    doc.appendChild(root);

    foreach (TupFrame *frame, k->frames.values())
             root.appendChild(frame->toXml(doc));

    return root;
}

TupScene *TupLayer::scene() const
{
    return static_cast<TupScene *>(parent());
}

TupProject *TupLayer::project() const
{
    return scene()->project();
}

int TupLayer::layerIndex()
{
    return k->index;
}

/*
int TupLayer::logicalIndexOf(TupFrame *frame) const
{
    return k->frames.logicalIndex(frame);
}
*/

int TupLayer::visualIndexOf(TupFrame *frame) const
{
    return k->frames.objectIndex(frame);
}

/*
int TupLayer::logicalIndex() const
{
    return scene()->logicalIndexOf(const_cast<TupLayer *>(this));
}
*/

int TupLayer::objectIndex() const
{
    return scene()->visualIndexOf(const_cast<TupLayer *>(this));
}

int TupLayer::framesTotal() const
{
    return k->framesCount;
}
