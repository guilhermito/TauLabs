TEMPLATE = lib
TARGET = Core
DEFINES += CORE_LIBRARY

QT += widgets
QT += xml \
    network \
    script \
    svg \
    sql
include(../../taulabsgcsplugin.pri)
include(../../libs/utils/utils.pri)
include(../../shared/scriptwrapper/scriptwrapper.pri)
include(coreplugin_dependencies.pri)
INCLUDEPATH *= dialogs \
    uavgadgetmanager \
    actionmanager
DEPENDPATH += dialogs \
    uavgadgetmanager \
    actionmanager
SOURCES += mainwindow.cpp \
    tabpositionindicator.cpp \
    generalsettings.cpp \
    uniqueidmanager.cpp \
    messagemanager.cpp \
    messageoutputwindow.cpp \
    versiondialog.cpp \
    iuavgadget.cpp \
    actionmanager/actionmanager.cpp \
    actionmanager/command.cpp \
    actionmanager/actioncontainer.cpp \
    coreplugin.cpp \
    variablemanager.cpp \
    threadmanager.cpp \
    coreimpl.cpp \
    plugindialog.cpp \
    mimedatabase.cpp \
    icore.cpp \
    settingsdatabase.cpp \
    eventfilteringmainwindow.cpp \
    iconnection.cpp \
    iuavgadgetconfiguration.cpp \
    uavconfiginfo.cpp \
    authorsdialog.cpp \
    boardmanager.cpp \
    iboardtype.cpp \
    idevice.cpp \
    globalmessaging.cpp \
    connectionmanager.cpp
HEADERS += mainwindow.h \
    tabpositionindicator.h \
    generalsettings.h \
    uniqueidmanager.h \
    messagemanager.h \
    messageoutputwindow.h \
    iuavgadget.h \
    iuavgadgetfactory.h \
    actionmanager/actioncontainer.h \
    actionmanager/actionmanager.h \
    actionmanager/command.h \
    actionmanager/actionmanager_p.h \
    actionmanager/command_p.h \
    actionmanager/actioncontainer_p.h \
    icontext.h \
    icore.h \
    imode.h \
    ioutputpane.h \
    coreconstants.h \
    iversioncontrol.h \
    iview.h \
    icorelistener.h \
    versiondialog.h \
    core_global.h \
    coreplugin.h \
    variablemanager.h \
    threadmanager.h \
    coreimpl.h \
    plugindialog.h \
    mimedatabase.h \
    settingsdatabase.h \
    eventfilteringmainwindow.h \
    iconnection.h \
    iuavgadgetconfiguration.h \
    uavconfiginfo.h \
    authorsdialog.h \
    iconfigurableplugin.h \
    boardmanager.h \
    iboardtype.h \
    idevice.h \
    globalmessaging.h \
    connectionmanager.h
FORMS += generalsettings.ui \
    uavgadgetoptionspage.ui \
    workspacesettings.ui
RESOURCES += core.qrc
unix:!macx { 
    images.files = images/taulabs_logo_*.png
    images.files = images/qtcreator_logo_*.png
    images.path = /share/pixmaps
    INSTALLS += images
}
OTHER_FILES += Core.pluginspec \
    coreplugin.json

HEADERS += slimgcs/windowbutton.h \
    slimgcs/windowtitlebar.h \
    slimgcs/mainwidget.h \
    slimgcs/tbssplashdialog.h
SOURCES += slimgcs/windowbutton.cpp \
    slimgcs/windowtitlebar.cpp \
    slimgcs/mainwidget.cpp \
    slimgcs/tbssplashdialog.cpp
FORMS += \
    slimgcs/mainwidget.ui \
    slimgcs/tbssplashdialog.ui
RESOURCES += \
    slimgcs/resource.qrc

include(gcsversioninfo.pri)
