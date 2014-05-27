INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tuppaletteimporter.h
SOURCES += tuppaletteimporter.cpp

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib 
TARGET = tupimport

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)
