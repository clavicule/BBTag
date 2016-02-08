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
    src/ui/bbtag_main_window.cpp \
    src/ui/main.cpp

HEADERS  += \
    include/ui/bbtag_main_window.h

RESOURCES += \
    resources/pixmaps_list.qrc
