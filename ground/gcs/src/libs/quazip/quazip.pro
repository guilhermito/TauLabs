TEMPLATE = lib
TARGET = QUAZIP_lib

INCLUDEPATH += ../glc_lib/3rdparty/zlib

DEFINES += QUAZIP_LIB_LIBRARY
include(../../taulabslibrary.pri)

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

SOURCES += ../glc_lib/3rdparty/zlib/adler32.c \
           ../glc_lib/3rdparty/zlib/compress.c \
           ../glc_lib/3rdparty/zlib/crc32.c \
           ../glc_lib/3rdparty/zlib/deflate.c \
           ../glc_lib/3rdparty/zlib/gzio.c \
           ../glc_lib/3rdparty/zlib/inffast.c \
           ../glc_lib/3rdparty/zlib/inflate.c \
           ../glc_lib/3rdparty/zlib/inftrees.c \
           ../glc_lib/3rdparty/zlib/trees.c \
           ../glc_lib/3rdparty/zlib/uncompr.c \
           ../glc_lib/3rdparty/zlib/zutil.c
