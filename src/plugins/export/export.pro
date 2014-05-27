unix {
    !include(../../../tupiglobal.pri) {
        error("Please configure first")
    }
}

SUBDIRS += imageplugin 

# SMIL plugin will be disabled temporary 
# smileplugin 

contains(DEFINES, HAVE_FFMPEG) {
        SUBDIRS += ffmpegplugin
}

contains(DEFINES, HAVE_THEORA) {
        SUBDIRS += theoraplugin
}

# Experimental code
SUBDIRS += apngplugin

CONFIG += ordered warn_on 
TEMPLATE = subdirs 
