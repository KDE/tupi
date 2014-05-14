INSTALLS += target 
target.path = /plugins/ 

HEADERS += imageplugin.h
SOURCES += imageplugin.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiimageplugin

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi
LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

include(../export_config.pri)
include(../../../../tupiglobal.pri)
