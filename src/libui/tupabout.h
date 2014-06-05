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

#ifndef TUPABOUT_H
#define TUPABOUT_H

#include "tglobal.h"
#include "tapplicationproperties.h"
#include "tabdialog.h"
#include "tanimwidget.h"

#include <QTextBrowser>
#include <QLabel>
#include <QPixmap>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>
#include <QScrollArea>
#include <QDir>

/**
 * @brief Class that handles the about dialog box
 *
 * <b>Date of Creation: July 28 - 2004.</b>\n
 * <b>Rewritten in May 30 - 2005</b>\n
 * This is a dialog box that contains info about the authors of the application,
 * the application itself and another related stuff. It's a subclass of QTabDialog,
 * and provides 6 Tabs:
 * - Copyrights
 * - Credits
 * - Acknowledgements
 * - SVN Log
 * - TupAbout Toonka Films
 * - GNU Public License
 *
 * Moreover, it provides an OK Button and a Cancel Button.
 */

class TUPI_EXPORT TupAbout : public TabDialog
{
    Q_OBJECT

    public:
        TupAbout(QWidget *parent);
        ~TupAbout();

    private:
        TAnimWidget *m_credits;
};

#endif
