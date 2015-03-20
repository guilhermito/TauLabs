TEMPLATE = lib
TARGET = Quazip

INCLUDEPATH += ./zlib

DEFINES += QUAZIP_LIB_LIBRARY
include(../../taulabslibrary.pri)

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

HEADERS +=  crypt.h \
            ioapi.h \
                    quazip.h \
                    quazipfile.h \
                    quazipfileinfo.h \
                    quazipnewinfo.h \
                    quazip_global.h

SOURCES += ioapi.c \
           quazip.cpp \
           quazipfile.cpp \
           quazipnewinfo.cpp \
           unzip.c \
           zip.c

SOURCES += ./zlib/adler32.c \
           ./zlib/compress.c \
           ./zlib/crc32.c \
           ./zlib/deflate.c \
           ./zlib/gzio.c \
           ./zlib/inffast.c \
           ./zlib/inflate.c \
           ./zlib/inftrees.c \
           ./zlib/trees.c \
           ./zlib/uncompr.c \
           ./zlib/zutil.c
