TEMPLATE = lib
TARGET = TauLabs
include(../../taulabsgcsplugin.pri)
include(../../plugins/uavobjects/uavobjects.pri)
include(../../plugins/uavobjectutil/uavobjectutil.pri)
include(../../plugins/uavobjectwidgetutils/uavobjectwidgetutils.pri)

OTHER_FILES += TauLabs.json
SLIM_GCS {
INCLUDEPATH+=../slimcoreplugin
}
!SLIM_GCS {
INCLUDEPATH+=../coreplugin
}
HEADERS += \
    taulabsplugin.h \
    sparky.h \
    sparkybgc.h \
    sparkybgcconfiguration.h \
    sparky2.h \
    taulink.h

SOURCES += \
    taulabsplugin.cpp \
    sparky.cpp \
    sparkybgc.cpp \
    sparkybgcconfiguration.cpp \
    sparky2.cpp \
    taulink.cpp

RESOURCES += \
    taulabs.qrc

FORMS += \
    sparkybgcconfiguration.ui
