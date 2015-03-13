
TEMPLATE = lib
QT += xml
QT += widgets
TARGET = UAVSettingsImportExport
DEFINES = UAVSETTINGSIMPORTEXPORT_LIBRARY
include(../../taulabsgcsplugin.pri)
include(uavsettingsimportexport_dependencies.pri)
include(../../libs/utils/utils.pri)

SLIM_GCS {
INCLUDEPATH+=../../../../../build/ground/slimgcs
}
!SLIM_GCS {
INCLUDEPATH+=../../../../../build/ground/gcs
}

HEADERS += uavsettingsimportexport.h \
    importsummary.h \
    uavsettingsimportexportfactory.h
SOURCES += uavsettingsimportexport.cpp \
    importsummary.cpp \
    uavsettingsimportexportfactory.cpp
 
OTHER_FILES += uavsettingsimportexport.pluginspec \
    uavsettingsimportexport.json

FORMS += \
    importsummarydialog.ui
