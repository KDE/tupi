QT += opengl core gui svg xml network

unix {
    !include(../tupconfig.pri) {
        error("Run ./configure first!")
    }
}

INSTALLS += target
target.path = /lib/

contains("DEFINES", "ADD_HEADERS") {
    INSTALLS += headers 
    headers.files += *.h
    headers.path = /include/tupicore
}

macx {
    CONFIG += plugin warn_on
}

HEADERS += talgorithm.h \
           tapplicationproperties.h \
           tconfig.h \
           tglobal.h \
           tipdatabase.h \
           txmlparserbase.h

SOURCES += talgorithm.cpp \
           tapplicationproperties.cpp \
           tconfig.cpp \
           tipdatabase.cpp \
           txmlparserbase.cpp

DEFINES += K_DEBUG
		   
#unix {
    contains(DEFINES, K_DEBUG) {
        HEADERS += tdebug.h
        SOURCES += tdebug.cpp

        win32 {
	        CONFIG += console
        }
	}
#}

*:!macx{
    CONFIG += warn_on dll
}

TEMPLATE = lib
TARGET = tupifwcore
