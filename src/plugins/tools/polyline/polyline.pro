INSTALLS += target
target.path = /plugins/

HEADERS += polylinetool.h \
           infopanel.h

SOURCES += polylinetool.cpp \
           infopanel.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupipolylinetool

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
