INCLUDEPATH += ../../shell

STORE_DIR = ../store 
LIBTUPI_DIR = ../libtupi
LIBBASE_DIR = ../libbase

include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
include($$LIBBASE_DIR/libbase.pri)


!include(../../tupiglobal.pri) {
         error("Run ./configure first!")
}

INCLUDEPATH += ../../shell

# QT += xml

