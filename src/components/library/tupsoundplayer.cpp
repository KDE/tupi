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

#include "tupsoundplayer.h"

struct TupSoundPlayer::Private
{
    QSlider *slider;
    QLabel *timer;
    TImageButton *playButton;
    bool playing;
};

TupSoundPlayer::TupSoundPlayer(QWidget *parent) : QFrame(parent), k(new Private)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setMidLineWidth(2);
    setLineWidth(1);

    k->playing = false;

    k->slider = new QSlider(Qt::Horizontal, this);
    k->slider->setRange(0, 100);

    k->timer = new QLabel("00:00");

    QBoxLayout *sliderLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    sliderLayout->addWidget(k->slider);
    sliderLayout->addWidget(k->timer);
    sliderLayout->setContentsMargins(0, 0, 0, 0);

    k->playButton = new TImageButton(QPixmap(THEME_DIR + "icons/play.png"), 33, this, true);
    k->playButton->setToolTip(tr("Play"));
    connect(k->playButton, SIGNAL(clicked()), this, SLOT(playFile()));

    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout->addStretch();
    buttonLayout->addWidget(k->playButton);
    buttonLayout->addStretch();
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->addLayout(sliderLayout);
    layout->addLayout(buttonLayout);
    layout->setContentsMargins(5, 5, 5, 5);
}

TupSoundPlayer::~TupSoundPlayer()
{
    delete k;
}

QSize TupSoundPlayer::sizeHint() const
{
    return QWidget::sizeHint().expandedTo(QSize(100, 100));
}

void TupSoundPlayer::playFile()
{
    if (!k->playing) {
        k->playButton->setIcon(QIcon(QPixmap(THEME_DIR + "icons/stop.png")));
        k->playing = true;
    } else {
        k->playButton->setIcon(QIcon(QPixmap(THEME_DIR + "icons/play.png")));
        k->playing = false;
    }
}

