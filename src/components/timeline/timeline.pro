INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tuptimeline.h \
           tuplayermanager.h \
           tuptlruler.h \
           tupframestable.h \ 
           tuplayerindex.h \
           tuplayercontrols.h

SOURCES += tuptimeline.cpp \
           tuplayermanager.cpp \
           tuptlruler.cpp \
           tupframestable.cpp \
           tuplayerindex.cpp \
           tuplayercontrols.cpp
*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupitimeline

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)
