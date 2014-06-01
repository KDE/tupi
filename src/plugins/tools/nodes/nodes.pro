QT += opengl core gui svg xml network

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}

INSTALLS += target 
target.path = /plugins/

HEADERS += nodestool.h
SOURCES += nodestool.cpp

CONFIG += plugin warn_on
TEMPLATE = lib
TARGET = tupinodestool

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)

unix {
    include(../tools_config.pri)
}

win32 {
    LIBBASE_DIR = ../../../libbase
    STORE_DIR = ../../../store
    LIBTUPI_DIR = ../../../libtupi
    COMMON_DIR = ../common

    include($$LIBBASE_DIR/libbase.pri)	
    include($$STORE_DIR/store.pri)
    include($$LIBTUPI_DIR/libtupi.pri)
	include($$COMMON_DIR/common.pri)
}