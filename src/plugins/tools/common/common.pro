INSTALLS += target 
target.path = /lib/ 

#headers.target = .
#headers.commands = cp *.h $(INSTALL_ROOT)/include/plugincommon
#headers.path = /include/plugincommon

macx {
    CONFIG += staticlib warn_on
}

HEADERS += buttonspanel.h \
           tweenmanager.h \
           stepsviewer.h \
           spinboxdelegate.h \
           target.h

SOURCES += buttonspanel.cpp \
           tweenmanager.cpp \
           stepsviewer.cpp \
           spinboxdelegate.cpp \
           target.cpp
*:!macx {
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupiplugincommon 

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(./tools_config.pri)

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}
