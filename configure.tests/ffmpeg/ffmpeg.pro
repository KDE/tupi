TEMPLATE = app
CONFIG -= moc
TARGET = ffmpeg

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

INCLUDEPATH += .
INCLUDEPATH += /usr/include/ffmpeg
DEFINES += __STDC_CONSTANT_MACROS
LIBS += -lavformat -lavcodec -lavutil

# Input
SOURCES += main.cpp
