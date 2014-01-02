# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src/plugins/tools/selecttool
# Target is a library:  

INSTALLS += target 
target.path = /plugins/

HEADERS += selecttool.h \
           node.h \
           nodemanager.h \ 
           infopanel.h
SOURCES += selecttool.cpp \
           node.cpp \
           nodemanager.cpp \ 
           infopanel.cpp

OTHER_FILES += selecttool.json

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupiselecttool

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
