QT += opengl core gui svg xml network

STORE_DIR = ../../store 
LIBTUPI_DIR = ../libtupi
LIBBASE_DIR = ../libbase

include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$LIBBASE_DIR/libbase.pri)

unix {
    !include(../../tupiglobal.pri) {
        error("Please configure first")
    }
}

INCLUDEPATH += ../../shell
