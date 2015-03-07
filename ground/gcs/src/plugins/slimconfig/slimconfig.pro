TEMPLATE = lib
TARGET = Config
DEFINES += CONFIG_LIBRARY
include(config_dependencies.pri)
INCLUDEPATH *= ../../libs/eigen

OTHER_FILES += Config.pluginspec \
    Config.json

HEADERS += configinputwidget.h \
    configplugin.h \
    configgadgetfactory.h \
    config_global.h

SOURCES += configinputwidget.cpp \
    configplugin.cpp \
    configgadgetfactory.cpp

FORMS +=
RESOURCES +=







