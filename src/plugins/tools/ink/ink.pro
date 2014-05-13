INSTALLS += target 
target.path = /plugins/

HEADERS += inktool.h \
           configurator.h
SOURCES += inktool.cpp \
           configurator.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiinktool

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

include(../../../../tupiglobal.pri)
