TEMPLATE = lib
QT += xml
QT += network
TARGET = Updater

DEFINES += UPDATER_LIBRARY
include(../../taulabsgcsplugin.pri)
include(../../libs/utils/utils.pri)
include(../../plugins/uavtalk/uavtalk.pri)
include(../../plugins/uavobjectutil/uavobjectutil.pri)

SLIM_GCS {
DEFINES+=SLIM_GCS
INCLUDEPATH+=../../../../../build/ground/slimgcs
}
!SLIM_GCS {
DEFINES+=FULL_GCS
INCLUDEPATH+=../../../../../build/ground/gcs
}

HEADERS += updaterplugin.h \
    updaterfactory.h \
    filedownloader.h
SOURCES += updaterplugin.cpp \
    updaterfactory.cpp \ 
    filedownloader.cpp
    
OTHER_FILES += updater.pluginspec \
    updater.json
