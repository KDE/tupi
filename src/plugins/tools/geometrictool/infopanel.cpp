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

#include "infopanel.h"
#include "tglobal.h"
#include "tdebug.h"

#include <QBoxLayout>
#include <QTextEdit>

InfoPanel::InfoPanel(InfoPanel::ToolType type, QWidget *parent) : QWidget(parent)
{
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *label = new QLabel(tr("Tips"));
    label->setAlignment(Qt::AlignHCenter); 
    layout->addWidget(label);

    mainLayout->addLayout(layout);

    QTextEdit *textArea = new QTextEdit; 

    // SQA: Check this code with several screen resolutions. It must looks good with everyone! 
    textArea->setFont(QFont("Arial", 8, QFont::Normal, false));

    if (type == InfoPanel::Line) {
        textArea->append("<p><b>" + tr("Mouse Double Click") + ":</b> " +  tr("Close the line path") + "</p>");
        textArea->append("<p><b>" + tr("Shift") + ":</b> " +  tr("Align line to horizontal/vertical axis") + "</p>"); 
    } else {
        textArea->append("<p><b>" + tr("Shift + Left Mouse Button") + ":</b> " +  tr("Set width/height proportional dimensions") + "</p>");
    }

    QString text = textArea->document()->toPlainText();
    int height = 150;

    textArea->setFixedHeight(height);

    mainLayout->addWidget(textArea);
    mainLayout->addStretch(2);
}

InfoPanel::~InfoPanel()
{
}

