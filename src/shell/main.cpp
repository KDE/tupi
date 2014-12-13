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

#include "tupapplication.h"
#include "tupmainwindow.h"
#include "tuptwitter.h"
#include "tapplicationproperties.h"
#include "tcollapsiblewidget.h"

#ifdef K_DEBUG
#ifdef Q_OS_WIN32
#include <QDebug>
#else
#include "tdebug.h"
#include "tupcrashhandler.h"
#endif
#endif

#ifdef ENABLE_TUPISTYLE
#include "twaitstyle.h"
#endif

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QMessageBox>
#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QDesktopWidget>
#include <QThread>
#include <QStyleFactory>

/**
 * Main class of the application.
 * This is the code where Tupi application starts.
 * @author David Cuadrado
*/

/**
 * This method is the first one invoked when Tupi is launched
 */

int main(int argc, char ** argv)
{
    TupApplication application(argc, argv);
    QString slash = QDir::separator();

#ifdef Q_OS_UNIX
#ifdef K_DEBUG
    TDebug::setOutputChannel();
    // Initializing the crash handler, very useful to catch bugs
    TupCrashHandler::init();
#endif
#endif

    // Setting the current version for Tupi
    kAppProp->setVersion(VERSION);
    kAppProp->setCodeName(CODE_NAME);
    kAppProp->setRevision(REVISION);

    // Time to define global variables for Tupi
    TCONFIG->beginGroup("General");
    QDir appDirPath(QApplication::applicationDirPath());
    if (! TCONFIG->isOk()) {
        #if defined(K_DEBUG)
            TCONFIG->setValue("Debug", "true");
        #else
            TCONFIG->setValue("Debug", "false");
        #endif

        #if defined(Q_OS_MAC)
            TCONFIG->setValue("Home", appDirPath.absolutePath());
        #else
            TCONFIG->setValue("Home", QString::fromLocal8Bit(::getenv("TUPI_HOME")));
        #endif
        
        #ifdef Q_OS_WIN32
            if (QSysInfo::windowsVersion() == QSysInfo::WV_XP) {
                QDir dir("C:\temp");
                if (!dir.exists()) {
                    if (!dir.mkdir("C:\temp")) {
                        #ifdef K_DEBUG
                            qDebug() << "main.cpp - Fatal error: WinXP issue!";
                        #endif
                        return 0;
                    }
                }
                TCONFIG->setValue("Cache", QDir::tempPath());
            } else {
                TCONFIG->setValue("Cache", QDir::tempPath());
            }
        #else
                TCONFIG->setValue("Cache", QDir::tempPath());    
        #endif
    }

#ifdef K_DEBUG
    QString debug = "main.cpp - CACHE path: " + TCONFIG->value("Cache").toString();
    #ifdef Q_OS_WIN32
        qWarning() << debug;
    #else
        tWarning() << debug;
    #endif
#endif
    
#if defined(Q_OS_MAC)
    kAppProp->setHomeDir(TCONFIG->value("Home").toString());
    kAppProp->setBinDir(appDirPath.absolutePath());
    kAppProp->setPluginDir(appDirPath.absolutePath() + slash + "plugins");
    kAppProp->setShareDir(appDirPath.absolutePath() + slash + "share");
#else
    kAppProp->setHomeDir(TCONFIG->value("Home").toString());
    kAppProp->setBinDir(QString::fromLocal8Bit(::getenv("TUPI_BIN")));
    kAppProp->setPluginDir(QString::fromLocal8Bit(::getenv("TUPI_PLUGIN")));
    kAppProp->setShareDir(QString::fromLocal8Bit(::getenv("TUPI_SHARE")));
#endif

    QString locale = QString(QLocale::system().name()).left(2);
    if (locale.length() < 2)
        locale = "en";

#ifdef Q_OS_WIN32
    QString xmlDir = kAppProp->shareDir() + "xml" + slash;
#else
    QString xmlDir = kAppProp->shareDir() + "data" + slash + "xml" + slash;
#endif
    QDir dir(xmlDir + locale + slash);
    if (!dir.exists())
        kAppProp->setDataDir(xmlDir + "en" + slash);
    else
        kAppProp->setDataDir(xmlDir + locale + slash);
        
    kAppProp->setThemeDir(kAppProp->shareDir() + "themes" + slash + "default" + slash);

    // Setting the repository directory (where the projects are saved)
    application.createCache(TCONFIG->value("Cache").toString());

    // Downloading maefloresta Twitter status
    TupTwitter *twitter = new TupTwitter();
    twitter->start();

    QStyle *style = QStyleFactory::create("fusion");
    QApplication::setStyle(style);

    if ((locale.compare("en") != 0) && ((locale.compare("es") == 0) || (locale.compare("pt") == 0))) {
        #ifdef Q_OS_WIN32
            QString langFile = kAppProp->shareDir() + "translations" + slash + "tupi_" + locale + ".qm";
        #else
            QString langFile = kAppProp->shareDir() + "data" + slash + "translations" + slash + "tupi_" + locale + ".qm";
        #endif
        if (QFile::exists(langFile)) {
            // Loading localization files...
            QTranslator *translator = new QTranslator;
            translator->load(langFile);
            application.installTranslator(translator);
        } else {
            #ifdef K_DEBUG
                QString msg = "main.cpp - Error: Can't open file -> " + langFile;
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif    
        }
    }

    TupMainWindow mainWindow(argc);
    mainWindow.showMaximized();

    // Looking for plugins for Tupi
    #ifdef K_DEBUG
        QString msg = "main.cpp - Loading plugins from: " + kAppProp->pluginDir();
        #ifdef Q_OS_WIN32
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif
    QApplication::addLibraryPath(kAppProp->pluginDir());

    // Loading visual components required for the Crash Handler
    #if defined(Q_OS_UNIX) && defined(K_DEBUG)
        CHANDLER->setConfig(DATA_DIR + "crashhandler.xml");
        CHANDLER->setImagePath(THEME_DIR + "icons/");
    #endif

    // If there is a second argument, it means to open a project from the command line
    if (argc == 2) {
        QString project = QString(argv[1]);
        if (project.endsWith(".tup") || project.endsWith(".TUP"))
            mainWindow.openProject(project);
    }

    // It's time to play with Tupi!
    return application.exec();
}
