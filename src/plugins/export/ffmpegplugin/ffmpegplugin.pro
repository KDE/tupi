INSTALLS += target
target.path = /plugins/
#INSTALL_ROOT = /tmp/test

HEADERS += tffmpegmoviegenerator.h \
           ffmpegplugin.h

SOURCES += tffmpegmoviegenerator.cpp \
           ffmpegplugin.cpp

CONFIG += plugin warn_on
TEMPLATE = lib 
TARGET = tupiffmpegplugin

INCLUDEPATH += ../../../libbase
INCLUDEPATH += ../../../store
INCLUDEPATH += ../../../libtupi
LIBS += -L../../../libbase
LIBS += -L../../../store
LIBS += -L../../../libtupi

FRAMEWORK_DIR = "../../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../export_config.pri)

unix {
    !include(../../../../tupiglobal.pri){
        error("Please run configure first")
    }
}

win32 {
    include(../../../../win.pri)
}
