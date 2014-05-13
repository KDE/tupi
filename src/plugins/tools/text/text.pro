INSTALLS += target 
target.path = /plugins/ 

HEADERS += texttool.h \
           textconfigurator.h 
SOURCES += texttool.cpp \
           textconfigurator.cpp 

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupitexttool

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
