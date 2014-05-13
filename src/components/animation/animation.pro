# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src/components/animation
# Target is a library: animation 

INSTALLS += target
target.path = /lib/

macx {
     CONFIG += staticlib warn_on
}

HEADERS += tupcamerawidget.h \
           tupcamerabar.h \
           tupscreen.h \
           tupcamerastatus.h \
           tupanimationspace.h

SOURCES += tupcamerawidget.cpp \
           tupcamerabar.cpp \
           tupscreen.cpp \
           tupcamerastatus.cpp \
           tupanimationspace.cpp

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupianimation 

EXPORT_DIR = ../export
include($$EXPORT_DIR/export.pri)

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)
