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

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)
