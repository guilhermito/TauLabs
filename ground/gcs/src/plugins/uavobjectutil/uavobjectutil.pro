TEMPLATE = lib
QT += widgets
TARGET = UAVObjectUtil
DEFINES += UAVOBJECTUTIL_LIBRARY
include(../../taulabsgcsplugin.pri)
include(uavobjectutil_dependencies.pri)

SLIM_GCS {
INCLUDEPATH+=../../../../../build/ground/slimgcs
}
!SLIM_GCS {
INCLUDEPATH+=../../../../../build/ground/gcs
}
HEADERS += uavobjectutil_global.h \
	uavobjectutilmanager.h \
    uavobjectutilplugin.h \
   devicedescriptorstruct.h

SOURCES += uavobjectutilmanager.cpp \
    uavobjectutilplugin.cpp \
    devicedescriptorstruct.cpp

OTHER_FILES += UAVObjectUtil.pluginspec \
    UAVObjectUtil.json
