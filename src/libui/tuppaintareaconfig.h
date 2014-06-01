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

#ifndef TUPPAINTAREACONFIG_H
#define TUPPAINTAREACONFIG_H

#include "tglobal.h"
#include "tcolorbutton.h"
#include "tconfig.h"

#ifdef K_DEBUG
#include "tdebug.h"
#endif

#include <QWidget>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

class TColorButton;

/**
 * @author David Cuadrado
*/
class TUPI_EXPORT TupPaintAreaConfig : public QWidget
{
    Q_OBJECT

    public:
        TupPaintAreaConfig(QWidget *parent = 0);
        ~TupPaintAreaConfig();
        
        QColor gridColor() const;
        QColor backgroundColor() const;
        QColor onionSkinColor() const;
        QColor onionSkinBackground() const;
        int gridSeparation() const;
        
    private:
        void setupPage();
        
    private:
        TColorButton *m_gridColor,*m_backgroundColor,*m_onionSkinColor,*m_onionSkinBackground;
        QSpinBox *m_gridSeparation;
};

#endif
