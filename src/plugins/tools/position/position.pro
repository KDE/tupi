INSTALLS += target 
target.path = /plugins/

HEADERS += tweener.h \
           configurator.h \
           settings.h
SOURCES += tweener.cpp \
           configurator.cpp \
           settings.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupipositiontool

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi
INCLUDEPATH += = ../common
LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi
LIBS += -L../common

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../tools_config.pri)

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}
~
