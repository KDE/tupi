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

#include "twizard.h"

TWizard::TWizard(QWidget *parent) : QDialog(parent)
{
    m_cancelButton = new QPushButton(tr("Cancel"));
    m_backButton = new QPushButton(tr("< &Back"));
    m_nextButton = new QPushButton(tr("Next >"));
    m_finishButton = new QPushButton(tr("&Finish"));

    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_backButton, SIGNAL(clicked()), this, SLOT(back()));
    connect(m_nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(m_finishButton, SIGNAL(clicked()), this, SLOT(finish()));

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addStretch(1);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_backButton);
    m_buttonLayout->addWidget(m_nextButton);
    m_buttonLayout->addWidget(m_finishButton);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(&m_history);
    m_mainLayout->addLayout(m_buttonLayout);
    setLayout(m_mainLayout);
}

TWizard::~TWizard()
{
}

TWizardPage *TWizard::addPage(TWizardPage *newPage)
{
    newPage->setParent(&m_history);
    newPage->show();

    m_history.addWidget(newPage);

    if (m_history.count() == 1) { // First Page
        newPage->setFocus();
        m_backButton->setEnabled(false);
        m_nextButton->setDefault(true);
    }

    m_nextButton->setEnabled(newPage->isComplete());

    connect(newPage, SIGNAL(completed()), this, SLOT(pageCompleted()));

    return newPage;
}

void TWizard::showPage(TWizardPage *page)
{
    m_history.setCurrentWidget(page);
}

void TWizard::showPage(int index)
{
    m_history.setCurrentIndex(index);
}

void TWizard::back()
{
    TWizardPage *current = qobject_cast<TWizardPage *>(m_history.currentWidget());
    if (current)
        current->aboutToBackPage();

    m_history.setCurrentIndex(m_history.currentIndex()-1);

    if (m_history.currentIndex() == 0) {
        m_nextButton->setEnabled(true);
        m_finishButton->setEnabled(false);
        m_backButton->setEnabled(false);
        m_nextButton->setDefault(true);
    } else {
        m_nextButton->setDefault(true);
        m_finishButton->setEnabled(false);
    }
}

void TWizard::next()
{
    TWizardPage *current = qobject_cast<TWizardPage *>(m_history.currentWidget());
    if (current)
        current->aboutToNextPage();

    m_history.setCurrentIndex(m_history.currentIndex()+1);

    if (m_history.currentIndex() == m_history.count()-1 && current->isComplete()) {
        m_nextButton->setEnabled(false);
        m_backButton->setEnabled(true);
        m_finishButton->setDefault(true);
    } else {
        m_finishButton->setEnabled(false);
    }

    pageCompleted();
}

void TWizard::finish()
{
    TWizardPage *current = qobject_cast<TWizardPage *>(m_history.currentWidget());
    if (current) 
        current->aboutToFinish();
    accept();
}

void TWizard::pageCompleted()
{
    TWizardPage *current = qobject_cast<TWizardPage *>(m_history.currentWidget());

    if (m_history.currentIndex() == m_history.count()-1)
        m_finishButton->setEnabled(current->isComplete());
    else
        m_nextButton->setEnabled(current->isComplete());
}

TWizardPage::TWizardPage(const QString &title, QWidget *parent) : TVHBox(parent)
{
    TVHBox *theTitle = new TVHBox(this, Qt::Vertical);
    new QLabel(title, theTitle);
    new TSeparator(theTitle);
    boxLayout()->setAlignment(theTitle, Qt::AlignTop);

    m_container = new QFrame(this);
    m_layout = new QGridLayout(m_container);

    m_image = new QLabel;
    m_layout->addWidget(m_image, 0, 0, Qt::AlignLeft);
    m_image->hide();

    new TSeparator(this);
    hide();
}

void TWizardPage::setPixmap(const QPixmap &px)
{
    m_image->setPixmap(px);
    m_image->show();
}

void TWizardPage::setWidget(QWidget *w)
{
    m_layout->addWidget(w, 0, 1);
}

TWizardPage::~TWizardPage() {};
