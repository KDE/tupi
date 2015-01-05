QT += opengl core gui svg xml network

INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
} else {
    CONFIG += dll warn_on
}

HEADERS += tupdebugwidget.h \
           tupdebugterm.h

SOURCES += tupdebugwidget.cpp \
           tupdebugterm.cpp

TEMPLATE = lib
TARGET = tupidebug

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)

STORE_DIR = ../../store/
INCLUDEPATH += $$STORE_DIR
LIBS += -L$$STORE_DIR -ltupistore

LIBBASE_DIR = ../../libbase/
INCLUDEPATH += $$LIBBASE_DIR
LIBS += -L$$LIBBASE_DIR -ltupibase

LIBTUPI_DIR = ../../libtupi/
INCLUDEPATH += $$LIBTUPI_DIR
LIBS += -L$$LIBTUPI_DIR -ltupi

!include(../../../tupiglobal.pri) {
         error("Run ./configure first!")
}
