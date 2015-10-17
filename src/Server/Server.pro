#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T13:19:12
#
#-------------------------------------------------


QT       += core

QT       -= gui

win32 {
    LIBS += -lpthreadGC2 -lws2_32
}

unix {
    LIBS += -lpthread
}

TARGET = SeaBattleServer
CONFIG   += console c++14
CONFIG   -= app_bundle

TEMPLATE = app



SOURCES += main.cpp

HEADERS += \
    logger.h
