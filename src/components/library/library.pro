# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./src/components/library
# Target is a library:  

INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tuplibrarywidget.h \
           tupitemmanager.h  \
           tupsymboleditor.h \
           tuptreedelegate.h \
           tupnewitemdialog.h

SOURCES += tuplibrarywidget.cpp \
           tupitemmanager.cpp  \
           tupsymboleditor.cpp \
           tuptreedelegate.cpp \
           tupnewitemdialog.cpp

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupilibrary

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)
