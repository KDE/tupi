SUBDIRS += scenes \
           colorpalette \
           library \
           timeline \
           exposure \
           export \
           import \
           help \
           # kinas \
           pen \
           paintarea \
           twitter \
           animation

unix {
    SUBDIRS += debug
}

CONFIG += ordered warn_on
TEMPLATE = subdirs

QT += xml opengl network
