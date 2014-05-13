TEMPLATE = app
CONFIG -= moc
TARGET = theora

macx {
    CONFIG -= app_bundle
    CONFIG += warn_on static console
}

INCLUDEPATH += .
LIBS += -ltheora -ltheoraenc -ltheoradec -logg

# Input
SOURCES += main.cpp
