QT += opengl core gui svg xml network

INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tupexposuresheet.h \
           tupexposureheader.h \
           tupexposuretable.h \
           tupscenetabwidget.h

SOURCES += tupexposuresheet.cpp \
           tupexposureheader.cpp \ 
           tupexposuretable.cpp \
           tupscenetabwidget.cpp
*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupiexposure

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)

unix {
    include(../components_config.pri)
}

win32 {
    STORE_DIR = ../../store/
    INCLUDEPATH += $$STORE_DIR
    LIBS += -L$$STORE_DIR/release/ -ltupistore

	LIBBASE_DIR = ../../libbase/
    INCLUDEPATH += $$LIBBASE_DIR
    LIBS += -L$$LIBBASE_DIR/release/ -ltupibase

	LIBTUPI_DIR = ../../libtupi/
    INCLUDEPATH += $$LIBTUPI_DIR
    LIBS += -L$$LIBTUPI_DIR/release/ -ltupi 
}