DEFINES += K_DEBUG HAVE_FFMPEG
contains(DEFINES, K_DEBUG) {
    CONFIG += console
}
