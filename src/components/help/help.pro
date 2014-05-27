INSTALLS += help 
help.files += help 
help.path = /data/ 

INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tuphelpwidget.h \
           tuphelpbrowser.h 
SOURCES += tuphelpwidget.cpp \
           tuphelpbrowser.cpp 
*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupihelp

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)
