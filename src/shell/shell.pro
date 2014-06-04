QT += opengl core gui svg xml network
QT += multimedia multimediawidgets printsupport
QT += widgets

macx {
    CONFIG += console static
    ICON = ../../launcher/icons/tupi.icns
    QMAKE_INFO_PLIST = ./Info.plist
    TARGET = ../../bin/Tupi

    INSTALLS += icons

    icons.target = ../../launcher/icons/tupi.png
    icons.commands = cp ../../launcher/icons/tupi.png $(INSTALL_ROOT)/pixmaps
    icons.path = /pixmaps/

    INSTALLS += target \
                tupidata \

    target.path = /bin/

    tupidata.target = data
    tupidata.commands = cp -r data/* $(INSTALL_ROOT)/data
    tupidata.path = /data/
}

unix:!mac {
    INSTALLS += tupidata \
                launcher \
                mime \
                target \
                desktop \
                icons \
                tupiman \
                copyright

    tupidata.target = data
    tupidata.commands = cp -r data/* $(INSTALL_ROOT)/data
    tupidata.path = /data/

    launcher.target = ../../launcher/tupi
    launcher.commands = cp ../../launcher/tupi $(INSTALL_ROOT)/bin; chmod 755 $(INSTALL_ROOT)/bin/tupi
    launcher.path = /bin/

    mime.target = ../../launcher/tupi.xml
    mime.commands = cp ../../launcher/tupi.xml $(INSTALL_ROOT)/share/mime/packages
    mime.path = /share/mime/packages/

    desktop.target = ../../launcher/tupi.desktop
    desktop.commands = cp ../../launcher/tupi.desktop $(INSTALL_ROOT)/applications
    desktop.path = /applications/

    icons.target = ../../launcher/icons/tupi.png
    icons.commands = cp ../../launcher/icons/tupi.png $(INSTALL_ROOT)/pixmaps
    icons.path = /pixmaps/

    tupiman.target = ../components/help/man/tupi.1.gz
    tupiman.commands = cp ../components/help/man/tupi.1.gz $(INSTALL_ROOT)/man1
    tupiman.path = /man1/

    copyright.target = ../components/help/man/copyright
    copyright.commands = cp ../components/help/man/copyright $(INSTALL_ROOT)/share/doc/tupi
    copyright.path = /tupi/

    target.path = /bin/
    TARGET = ../../bin/tupi.bin
}

HEADERS += tupmainwindow.h \
           tupstatusbar.h \
           tupnewproject.h \
           # tupsplash.h \
           # tupcrashhandler.h \
           # tupcrashwidget.h \
           tupapplication.h \
           tuplocalprojectmanagerhandler.h

SOURCES += main.cpp \
           tupmainwindow.cpp \
           tupstatusbar.cpp \
           tupnewproject.cpp \
           # tupsplash.cpp \
           # tupcrashhandler.cpp \
           # tupcrashwidget.cpp \
           tupapplication.cpp \
           tupmainwindow_gui.cpp \
           tuplocalprojectmanagerhandler.cpp

unix {
    HEADERS += tupcrashhandler.h \
               tupcrashwidget.h
    SOURCES += tupcrashhandler.cpp \
               tupcrashwidget.cpp 
}
		   		   
CONFIG += warn_on
TEMPLATE = app

linux-g {
    TARGETDEPS += ../libtupi/libtupi.so \
  ../libui/libtupiui.so \
  ../store/libtupistore.so \
  ../net/libtupinet.so \
  ../components/paintarea/libtupipaintarea.so \
  ../components/pen/libtupipen.so \
  # ../components/kinas/libtupikinas.so \
  ../components/help/libtupihelp.so \
  ../components/import/libtupimport.so \
  ../components/export/libtupiexport.so \
  ../components/exposure/libtupiexposure.so \
  ../components/timeline/libtupitimeline.so \
  ../components/library/libtupilibrary.so \
  ../components/colorpalette/libtupicolorpalette.so \
  ../components/scenes/libtupiscenes.so \
  ../components/twitter/libtupitwitter.so
}

FRAMEWORK_DIR = ../framework
include($$FRAMEWORK_DIR/framework.pri)

unix {
    !include(../../tupiglobal.pri) {
        error("Please run configure first")
    }
}

win32 {
    TARGET = tupi
    DEFINES += VERSION=\\\"0.2\\\" CODE_NAME=\\\"Argentum\\\" REVISION=\\\"git04\\\"
    include(../../win.pri)
    include(../../quazip.win.pri)
}

include(shell_config.pri)
