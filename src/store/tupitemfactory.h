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

#ifndef TUPITEMFACTORY_H
#define TUPITEMFACTORY_H

#include "txmlparserbase.h"
#include "tupglobal_store.h"

#include <QStack>
#include <QPen>
#include <QBrush>

class QGraphicsItem;
class TupLibrary;

/**
 * @author David Cuadrado
*/

class STORE_EXPORT TupItemFactory : public TXmlParserBase
{
    public:
        enum Type
        {
            Vectorial = 1,
            Library 
        };

    	TupItemFactory();
    	~TupItemFactory();
    	
    	void setLibrary(const TupLibrary *library);
    	
    	/**
    	 * Analiza etiquetas de apertura del documento XML
    	 */
    	bool startTag(const QString& qname, const QXmlAttributes& atts);
    	void text(const QString &ch);
    	
    	/**
    	 * Analiza etiquetas de cierre del documento XML
    	 */
    	bool endTag(const QString& qname);
    	
    	QGraphicsItem *create(const QString &xml);
    	bool loadItem(QGraphicsItem *item, const QString &xml);

        QString itemID(const QString &xml);

        TupItemFactory::Type type();
    	
    private:
    	void setItemPen(const QPen &pen);
    	void setItemBrush(const QBrush &brush);
    	void setItemGradient(const QGradient& gradient, bool brush);
    	
    	QPen itemPen() const;
    	QBrush itemBrush() const;
    	
    	QGraphicsItem* createItem(const QString &xml);
    	
    private:
    	struct Private;
    	Private *const k;
};

#endif
