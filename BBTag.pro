#-------------------------------------------------
#
# Project created by QtCreator 2016-02-06T19:02:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BBTag
TEMPLATE = app

INCLUDEPATH += ./include

SOURCES += \
    src/ui/main.cpp \
    src/core/tag_model.cpp \
    src/core/tag_item.cpp \
    src/ui/main_window.cpp \
    src/ui/tag_viewer.cpp \
    src/ui/tag_scroll_view.cpp \
    src/core/tag_io.cpp

HEADERS  += \
    include/core/tag_model.h \
    include/core/tag_item.h \
    include/ui/main_window.h \
    include/ui/tag_viewer.h \
    include/ui/tag_scroll_view.h \
    include/core/tag_io.h

RESOURCES += \
    resources/pixmaps_list.qrc
