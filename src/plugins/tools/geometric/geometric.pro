INSTALLS += target 
target.path = /plugins/

HEADERS += geometrictool.h \
           infopanel.h 
SOURCES += geometrictool.cpp \
           infopanel.cpp

CONFIG += plugin
TEMPLATE = lib 
TARGET = tupigeometrictool

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
