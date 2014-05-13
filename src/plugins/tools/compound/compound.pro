INSTALLS += target 
target.path = /plugins/

HEADERS += tweener.h \
           configurator.h \
           tweenerpanel.h \
           tweenertable.h \
           positionsettings.h

SOURCES += tweener.cpp \
           # buttonspanel.cpp \
           configurator.cpp \
           tweenerpanel.cpp \
           tweenertable.cpp \
           positionsettings.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupicompoundtool

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
