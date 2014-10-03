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

#include "tuppapagayodialog.h"

struct TupPapagayoDialog::Private
{
    QLineEdit *filePath;
    QLineEdit *imagesPath;
    QLineEdit *soundPath;
};

TupPapagayoDialog::TupPapagayoDialog() : QDialog(), k(new Private)
{
    setWindowTitle(tr("Import Papagayo project"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/papagayo.png")));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QHBoxLayout *blockLayout = new QHBoxLayout;
    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    QVBoxLayout *textLayout = new QVBoxLayout;

    QPushButton *fileButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "papagayo.png")), " " + tr("&Load PGO File"), this); 
    connect(fileButton, SIGNAL(clicked()), this, SLOT(openFileDialog()));

    QPushButton *imagesButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "bitmap_array.png")), " " + tr("Load &Images"), this);
    connect(imagesButton, SIGNAL(clicked()), this, SLOT(openImagesDialog()));

    QPushButton *soundButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "bitmap_array.png")), " " + tr("Load &Sound"), this);
    connect(soundButton, SIGNAL(clicked()), this, SLOT(openSoundDialog()));

    buttonsLayout->addWidget(fileButton);
    buttonsLayout->addWidget(imagesButton);
    buttonsLayout->addWidget(soundButton);

    k->filePath = new QLineEdit();
    k->filePath->setReadOnly(true);
    k->imagesPath = new QLineEdit();
    k->imagesPath->setReadOnly(true);
    k->soundPath = new QLineEdit();
    k->soundPath->setReadOnly(true);

    textLayout->addWidget(k->filePath);
    textLayout->addWidget(k->imagesPath);
    textLayout->addWidget(k->soundPath);

    blockLayout->addLayout(buttonsLayout);
    blockLayout->addLayout(textLayout);

    layout->addLayout(blockLayout);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok 
                                | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttons, SIGNAL(accepted()), this, SLOT(checkRecords()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    layout->addWidget(buttons, 0, Qt::AlignCenter);
}

TupPapagayoDialog::~TupPapagayoDialog()
{
    delete k;
}

void TupPapagayoDialog::openFileDialog()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Load Papagayo project"), QDir::homePath(), tr("Papagayo Project (*.pgo)"));
    k->filePath->setText(file);
}

void TupPapagayoDialog::openImagesDialog()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the images directory..."), getenv("HOME"), 
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    k->imagesPath->setText(path);
}

void TupPapagayoDialog::openSoundDialog()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Load sound file"), QDir::homePath(), tr("Sound file (*.ogg *.wav *.mp3)"));
    k->soundPath->setText(file);
}

void TupPapagayoDialog::checkRecords()
{
    if (k->filePath->text().length() == 0) {
        TOsd::self()->display(tr("Error"), tr("PGO path is unset!"), TOsd::Error);
        return;
    }

    if (k->imagesPath->text().length() == 0) {
        TOsd::self()->display(tr("Error"), tr("Images directory is unset!"), TOsd::Error);
        return;
    }

    if (k->soundPath->text().length() == 0) {
        TOsd::self()->display(tr("Error"), tr("Sound path is unset!"), TOsd::Error);
        return;
    }

    accept();
}

QString TupPapagayoDialog::getPGOFile() const
{
    return k->filePath->text();
}

QString TupPapagayoDialog::getImagesFile() const
{
    return k->imagesPath->text();
}

QString TupPapagayoDialog::getSoundFile() const
{
    return k->soundPath->text();
}

