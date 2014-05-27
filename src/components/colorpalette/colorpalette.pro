INSTALLS += tpal 

tpal.target = palettes/*.tpal
tpal.commands = test -d $(INSTALL_ROOT)/data/palettes/ || mkdir $(INSTALL_ROOT)/data/palettes; \
                cp palettes/*.tpal $(INSTALL_ROOT)/data/palettes/
tpal.path = /data/palettes/

INSTALLS += target
target.path = /lib/

macx {
    CONFIG += staticlib warn_on
}

HEADERS += tupcolorpicker.h \
           tupcolorpalette.h \
           tupviewcolorcells.h \
           tupluminancepicker.h \
           tupcellscolor.h \
           tuppaletteparser.h \
           tupcolorvalue.h
SOURCES += tupcolorpicker.cpp \
           tupcolorpalette.cpp \
           tupviewcolorcells.cpp \
           tupluminancepicker.cpp \
           tupcellscolor.cpp \
           tuppaletteparser.cpp \
           tupcolorvalue.cpp

*:!macx{
    CONFIG += dll warn_on
}

TEMPLATE = lib
TARGET = tupicolorpalette

INCLUDEPATH += ../../libbase
INCLUDEPATH += ../../store
INCLUDEPATH += ../../libtupi
LIBS += -L../../libbase
LIBS += -L../../store
LIBS += -L../../libtupi

FRAMEWORK_DIR = "../../framework"
include($$FRAMEWORK_DIR/framework.pri)
include(../components_config.pri)

