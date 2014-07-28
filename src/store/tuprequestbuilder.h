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

#ifndef TUPREQUESTBUILDER_H
#define TUPREQUESTBUILDER_H

#include "tglobal.h"
#include "tuplibraryobject.h"
#include "tupproject.h"

#include <QString>
#include <QDomDocument>
#include <QVariant>

class TupProjectRequest;
class TupProjectResponse;

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupRequestBuilder
{
    protected:
        TupRequestBuilder();
        
    public:
        enum ItemType 
        {
            Graphic = 1000,
            Svg
        };

        ~TupRequestBuilder();
        
        static TupProjectRequest createItemRequest(int sceneIndex, int layerIndex, int frameIndex, int itemIndex, QPointF point, TupProject::Mode spaceMode, 
                                                  TupLibraryObject::Type type, int action, const QVariant &arg = QString(), const QByteArray &data = QByteArray());
        
        static TupProjectRequest createFrameRequest(int sceneIndex, int layerIndex, int frameIndex, int action, const QVariant &arg= QString(), const QByteArray &data = QByteArray());
        
        static TupProjectRequest createLayerRequest(int sceneIndex, int layerIndex, int action, const QVariant &arg= QString(), const QByteArray &data = QByteArray());
        
        static TupProjectRequest createSceneRequest(int sceneIndex, int action, const QVariant &arg= QString(), const QByteArray &data = QByteArray());
        
        static TupProjectRequest createLibraryRequest(int actionId, const QVariant &arg, TupLibraryObject::Type type, TupProject::Mode spaceMode = TupProject::FRAMES_EDITION,
                                                      const QByteArray &data = QByteArray(), const QString &folder = QString(), int scene = -1, int layer = -1, int frame = -1);
        
        static TupProjectRequest fromResponse(TupProjectResponse *response);
        
    private:
        static void appendData(QDomDocument &doc, QDomElement &element, const QByteArray &data);
};

#endif
