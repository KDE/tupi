INSTALLS += target 
target.path = /plugins/

HEADERS += selectiontool.h \
           node.h \
           nodemanager.h \ 
           settings.h

SOURCES += selectiontool.cpp \
           node.cpp \
           nodemanager.cpp \ 
           settings.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupiselectiontool

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
