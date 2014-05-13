# INCLUDEPATH += /usr/include/QtMobility /usr/include/QtMultimediaKit
INCLUDEPATH += /usr/include/qt5/QtMultimedia /usr/include/qt5/QtMultimediaWidgets
INCLUDEPATH += $$PAINTAREA_DIR 
LIBS += -L$$PAINTAREA_DIR -ltupipaintarea

linux-g++ {
PRE_TARGETDEPS += $$PAINTAREA_DIR/libtupipaintarea.so
}

