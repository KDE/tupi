TEMPLATE = app
CONFIG -= moc
TARGET = quazip 
DEPENDPATH += .

macx {
    INCLUDEPATH += /usr/local/include
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

unix:!macx {
  INCLUDEPATH += /usr/include/quazip
}

LIBS += -lquazip-qt5 -lz

# Input
SOURCES += main.cpp
