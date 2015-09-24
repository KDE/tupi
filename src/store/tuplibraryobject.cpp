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

#include "tuplibraryobject.h"
#include "tupitemfactory.h"
#include "tuppixmapitem.h"
// #include "tupitemgroup.h"
// #include "taudioplayer.h"

struct TupLibraryObject::Private
{
    TupLibraryObject::Type type;
    QVariant data;
    QString dataPath;
    QString symbolName;
    QString smallId;
    QString extension;
    QByteArray rawData; 
};

TupLibraryObject::TupLibraryObject(QObject *parent) : QObject(parent), k(new Private)
{
}

TupLibraryObject::~TupLibraryObject()
{
    delete k;
}

/**
 * Items, pics, text, etc...
 * @param data 
 */
void TupLibraryObject::setData(const QVariant &data)
{
    k->data = data;
}

QVariant TupLibraryObject::data() const
{
    return k->data;
}

void TupLibraryObject::setDataPath(const QString &path)
{
    k->dataPath = path;
}

QString TupLibraryObject::dataPath() const
{
    return k->dataPath;
}

void TupLibraryObject::setType(TupLibraryObject::Type type)
{
    k->type = type;
}

TupLibraryObject::Type TupLibraryObject::type() const
{
    return k->type;
}

void TupLibraryObject::setSymbolName(const QString &name)
{
    k->symbolName = name;
    k->symbolName.replace(QDir::separator(), "-");
    k->smallId = k->symbolName.section('.', 0, 0);
    k->extension = k->symbolName.section('.', 1, 1).toUpper();
}

QString TupLibraryObject::symbolName() const
{
    return k->symbolName;
}

QString TupLibraryObject::smallId() const
{
    return k->smallId;
}

QString TupLibraryObject::extension() const
{
    return k->extension;
}

void TupLibraryObject::fromXml(const QString &xml)
{
    QDomDocument document;
    if (!document.setContent(xml)) {
        #ifdef K_DEBUG
            QString msg = "TupLibraryObject::fromXml() - Fatal Error: Invalid XML structure!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }
    
    QDomElement objectTag = document.documentElement();
    if (objectTag.tagName() == "object") {
        setSymbolName(objectTag.attribute("id"));
        if (k->symbolName.isEmpty()) {
            #ifdef K_DEBUG
                QString msg = "TupLibraryObject::fromXml - Fatal Error: Symbol name is empty!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return;
        }
       
        bool isOk = false; 
        int index = objectTag.attribute("type").toInt(&isOk);
        if (isOk) {
            k->type = TupLibraryObject::Type(index);
        } else {
            #ifdef K_DEBUG
                QString msg = "TupLibraryObject::fromXml - Fatal Error: Invalid object type!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return;
        }

        switch (k->type) {
                case TupLibraryObject::Item:
                case TupLibraryObject::Text:
                     {
                         QDomElement objectData = objectTag.firstChild().toElement();
                         if (!objectTag.isNull()) {
                             QString data;
                             {
                                 QTextStream ts(&data);
                                 ts << objectData;
                             }

                             QByteArray array = data.toLocal8Bit();
                             if (!array.isEmpty() && !array.isNull()) {
                                 loadRawData(array);
                             } else {
                                 #ifdef K_DEBUG
                                     QString msg = "TupLibraryObject::fromXml() - Object data is empty! -> " + k->symbolName;
                                     #ifdef Q_OS_WIN32
                                         qDebug() << msg;
                                     #else
                                         tError() << msg;
                                     #endif
                                 #endif
                                 return;
                             }
                         } else {
                             #ifdef K_DEBUG
                                 QString msg = "TupLibraryObject::fromXml() - Fatal Error: Object data from xml is NULL -> " + k->symbolName;
                                 #ifdef Q_OS_WIN32
                                     qDebug() << msg;
                                 #else
                                     tError() << msg;
                                 #endif
                             #endif
                             return;
                         }
                     }
                break;
                case TupLibraryObject::Image:
                case TupLibraryObject::Svg:
                case TupLibraryObject::Sound:
                     {
                         k->dataPath = objectTag.attribute("path");
                     }
                break;
                default:
                     {
                         #ifdef K_DEBUG
                             QString msg = "TupLibraryObject::fromXml() - Unknown object type: " + QString::number(k->type);
                             #ifdef Q_OS_WIN32
                                 qWarning() << msg;
                             #else
                                 tWarning() << msg;
                             #endif
                         #endif

                         return;
                     }
                break;
        }
    }
}

QDomElement TupLibraryObject::toXml(QDomDocument &doc) const
{
    QDomElement object = doc.createElement("object");
    object.setAttribute("id", k->symbolName);
    object.setAttribute("type", k->type);
    
    QFileInfo finfo(k->dataPath);
    
    switch (k->type) {
            case Text:
            case Item:
            {
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (TupAbstractSerializable *serializable = dynamic_cast<TupAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }
            }
            break;
            case Svg:
            {
                 QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(k->data);
            
                 if (item) {
                     if (TupAbstractSerializable *serializable = dynamic_cast<TupAbstractSerializable *>(item))
                         object.appendChild(serializable->toXml(doc));
                 }

                 object.setAttribute("path", finfo.fileName());
            }
            break;
            case Image:
            case Sound:
            {
                 object.setAttribute("path", finfo.fileName());
            }
            break;
            default:
            {
            }
            break;
    }
    
    return object;
}

bool TupLibraryObject::loadRawData(const QByteArray &data)
{
    k->rawData = data;

    switch (k->type) {
            case TupLibraryObject::Item:
            {
                 TupItemFactory factory;
                 QGraphicsItem *item = factory.create(QString::fromLocal8Bit(data));
                 setData(QVariant::fromValue(item));
            }
            break;
            case TupLibraryObject::Text:
            {
                 setData(QString::fromLocal8Bit(data));
            }
            break;
            case TupLibraryObject::Image:
            {
                 QPixmap pixmap;
                 bool isOk = pixmap.loadFromData(data);

                 if (!isOk) {
                     #ifdef K_DEBUG
                         QString msg = "TupLibraryObject::loadRawData() - [ Fatal Error ] - Can't load image -> " + k->symbolName;
                         #ifdef Q_OS_WIN32
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                     return false;
                 }

                 TupPixmapItem *item = new TupPixmapItem;
                 item->setPixmap(pixmap);
                 setData(QVariant::fromValue(static_cast<QGraphicsItem *>(item)));
            }
            break;
            case TupLibraryObject::Svg:
            {
                 QString item(data);
                 setData(QVariant::fromValue(item));
            }
            break;
            case TupLibraryObject::Sound:
            {
                 setData(QVariant::fromValue(data));
            }
            break;
            default:
            {
                 return false;
            }
            break;
    }
    
    return true;
}

bool TupLibraryObject::loadDataFromPath(const QString &dataDir)
{
    switch (k->type) {
            case TupLibraryObject::Image:
            {
                 k->dataPath = dataDir + QDir::separator() + "images" + QDir::separator() + k->dataPath;
            }
            break;
            case TupLibraryObject::Sound:
            {
                 k->dataPath = dataDir + QDir::separator() + "audio" + QDir::separator() + k->dataPath;
            }
            break;
            case TupLibraryObject::Svg:
            {
                 k->dataPath = dataDir + QDir::separator() + "svg" + QDir::separator() + k->dataPath;
            }
            break;
            default: 
                 return false; 
            break;
    }

    loadData(k->dataPath);
    
    return true;
}

bool TupLibraryObject::loadData(const QString &path)
{
    switch (k->type) {
            case TupLibraryObject::Image:
            case TupLibraryObject::Svg:
            case TupLibraryObject::Sound:
            {
                 QFile file(path);
                 if (file.exists()) {
                     if (file.open(QIODevice::ReadOnly)) {
                         QByteArray array = file.readAll(); 
                         #ifdef K_DEBUG
                             QString msg1 = "TupLibraryObject::loadData() - Object path: " + path;
                             QString msg2 = "TupLibraryObject::loadData() - Object size: " + QString::number(array.size());
                             #ifdef Q_OS_WIN32
                                 qWarning() << msg1;
                                 qWarning() << msg2;
                             #else
                                 tWarning() << msg1;
                                 tWarning() << msg2;
                             #endif
                         #endif
                         if (!array.isEmpty() && !array.isNull()) {
                             loadRawData(array);
                         } else {
                             #ifdef K_DEBUG
                                 QString msg = "TupLibraryObject::loadData() - Warning: Image file is empty -> " + path;
                                 #ifdef Q_OS_WIN32
                                     qDebug() << msg;
                                 #else
                                     tError() << msg;
                                 #endif
                             #endif
                             return false;
                         }
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "TupLibraryObject::loadData() - Fatal Error: Can't access image file -> " + path; 
                             #ifdef Q_OS_WIN32
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                         return false;
                     }
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "TupLibraryObject::loadData() - Fatal Error: Image file doesn't exist -> " + path;
                         #ifdef Q_OS_WIN32
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                     return false;
                 }
            }
            break;
            /*
            case TupLibraryObject::Sound:
            {
            }
            break;
            */
            default:
                 return false;
            break;
    }

    return true;
}

void TupLibraryObject::saveData(const QString &dataDir)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupLibraryObject::saveData()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    switch (k->type) {
            case TupLibraryObject::Sound:
            {
                 QString path = dataDir + QDir::separator() + "audio" + QDir::separator();
            
                 if (! QFile::exists(path)) {
                     QDir dir;
                     dir.mkpath(path);
                 }
           
                 k->dataPath = path + k->symbolName;

                 QFile file(k->dataPath);
                 if (file.open(QIODevice::WriteOnly)) {
                     qint64 isOk = file.write(k->rawData);
                     file.close();

                     if (isOk == -1) {
                         #ifdef K_DEBUG
                             QString msg = "TupLibraryObject::saveData() - [ Fatal Error ] - Can't save file -> " + k->dataPath;
                             #ifdef Q_OS_WIN32
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                         return;
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "TupLibraryObject::saveData() - Image file saved successfully -> " + k->dataPath;
                             #ifdef Q_OS_WIN32
                                 qWarning() << msg;
                             #else
                                 tWarning() << msg;
                             #endif
                         #endif
                     }
                 }
            }
            break;
            case TupLibraryObject::Svg:
            {
                 QString saved = dataDir + QDir::separator() + "svg" + QDir::separator();

                 if (! QFile::exists(saved)) {
                     QDir dir;
                     dir.mkpath(saved);
                 }

                 QFile file(saved + k->symbolName);
                 if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                     return;

                 QTextStream out(&file);
                 out << qvariant_cast<QString>(k->data);

                 k->dataPath = saved + k->symbolName;
            }
            break;
            case TupLibraryObject::Image:
            {
                 QString destination = dataDir + QDir::separator() + "images" + QDir::separator();
            
                 if (!QFile::exists(destination)) {
                     QDir dir;
                     dir.mkpath(destination);

                     #ifdef K_DEBUG
                         QString msg = "TupLibraryObject::saveData() - Creating directory -> " + destination;
                         #ifdef Q_OS_WIN32
                             qWarning() << msg;
                         #else
                             tWarning() << msg;
                         #endif
                     #endif
                 }

                 k->dataPath = destination + k->symbolName;

                 QFile file(k->dataPath);
                 if (!file.open(QIODevice::WriteOnly)) {
                     #ifdef K_DEBUG
                         QString msg = "TupLibraryObject::saveData() - [ Fatal Error ] - Insufficient permissions to save file -> " + destination + k->symbolName;
                         #ifdef Q_OS_WIN32
                             qDebug() << msg;
                         #else
                             tError() << msg;
                         #endif
                     #endif
                     return;
                 } else {
                     qint64 isOk = file.write(k->rawData);
                     file.close();

                     if (isOk == -1) {
                         #ifdef K_DEBUG
                             QString msg = "TupLibraryObject::saveData() - [ Fatal Error ] - Can't save file -> " + destination + k->symbolName;
                             #ifdef Q_OS_WIN32
                                 qDebug() << msg;
                             #else
                                 tError() << msg;
                             #endif
                         #endif
                         return;
                     } else {
                         #ifdef K_DEBUG
                             QString msg = "TupLibraryObject::saveData() - Image file saved successfully -> " + destination + k->symbolName;
                             #ifdef Q_OS_WIN32
                                 qWarning() << msg;
                             #else
                                 tWarning() << msg;
                             #endif
                         #endif
                     }
                 }
            }
            break;
            default: 
            break;
    }
}
