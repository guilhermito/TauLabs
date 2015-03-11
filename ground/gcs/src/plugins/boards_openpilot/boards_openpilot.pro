TEMPLATE = lib
TARGET = OpenPilot
include(../../taulabsgcsplugin.pri)
include(../../plugins/uavobjects/uavobjects.pri)
include(../../plugins/uavobjectutil/uavobjectutil.pri)
include(../../plugins/uavobjectwidgetutils/uavobjectwidgetutils.pri)

OTHER_FILES += OpenPilot.pluginspec \
                OpenPilot.json

HEADERS += \
    openpilotplugin.h \
    coptercontrol.h \
    config_cc_hw_widget.h \
    revomini.h \
    pipxtreme.h

SOURCES += \
    openpilotplugin.cpp \
    coptercontrol.cpp \
    config_cc_hw_widget.cpp \
    revomini.cpp \
    pipxtreme.cpp

RESOURCES += \
    openpilot.qrc \

FORMS += \
    cc_hw_settings.ui
