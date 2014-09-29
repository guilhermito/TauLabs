#-------------------------------------------------
#
# Project created by QtCreator 2014-09-29T16:23:39
#
#-------------------------------------------------

QT       += core \
            network

QT       -= gui

TARGET = ForumAutotuneExtract
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../libs/utils/phpbb.cpp \
    forumextract.cpp

HEADERS += \
    ../../libs/utils/phpbb.h \
    forumextract.h
