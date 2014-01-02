
QT += xml opengl svg

COLORPALETTE_DIR = ../components/colorpalette
ANIMATION_DIR = ../components/animation
PAINTAREA_DIR = ../components/paintarea
PEN_DIR = ../components/pen
KINAS_DIR = ../components/kinas 
HELP_DIR = ../components/help 
IMPORT_DIR = ../components/import 
EXPORT_DIR = ../components/export 
EXPOSURE_DIR = ../components/exposure 
TIMELINE_DIR = ../components/timeline 
DEBUG_DIR = ../components/debug
LIBRARY_DIR = ../components/library
SCENES_DIR = ../components/scenes 
TWITTER_DIR = ../components/twitter
LIBUI_DIR = ../libui
STORE_DIR = ../store 
LIBTUPI_DIR = ../libtupi
# QUAZIP_DIR = ../../3rdparty/quazip 
LIBBASE_DIR = ../libbase
NET_DIR = ../net

include($$COLORPALETTE_DIR/colorpalette.pri)
include($$PAINTAREA_DIR/paintarea.pri)
include($$ANIMATION_DIR/animation.pri)
include($$PEN_DIR/pen.pri)
include($$KINAS_DIR/kinas.pri)
include($$HELP_DIR/help.pri)
include($$IMPORT_DIR/import.pri)
include($$EXPORT_DIR/export.pri)
include($$EXPOSURE_DIR/exposure.pri)
include($$TIMELINE_DIR/timeline.pri)
include($$DEBUG_DIR/debug.pri)
include($$LIBRARY_DIR/library.pri)
include($$SCENES_DIR/scenes.pri)
include($$TWITTER_DIR/twitter.pri)
include($$LIBUI_DIR/libui.pri)
include($$STORE_DIR/store.pri)
include($$LIBTUPI_DIR/libtupi.pri)
# include($$QUAZIP_DIR/quazip.pri)
include($$LIBBASE_DIR/libbase.pri)
include($$NET_DIR/net.pri)

