# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS   = plugin_coreplugin

# Blank Template Plugin, not compiled by default
#SUBDIRS += plugin_donothing
#plugin_donothing.subdir = donothing
#plugin_donothing.depends = plugin_coreplugin

!CONFIG(SLIM_GCS) {
# Core plugin
plugin_coreplugin.subdir = coreplugin
}
CONFIG(SLIM_GCS) {
# Slim Core plugin
plugin_coreplugin.subdir = slimcoreplugin
}

!CONFIG(SLIM_GCS) {
# Empty UAVGadget - Default for new splits
plugin_emptygadget.subdir = emptygadget
plugin_emptygadget.depends = plugin_coreplugin
SUBDIRS += plugin_emptygadget

# Debug Gadget plugin
plugin_debuggadget.subdir = debuggadget
plugin_debuggadget.depends = plugin_coreplugin
SUBDIRS += plugin_debuggadget
}

!CONFIG(SLIM_GCS) {
# Welcome plugin
plugin_welcome.subdir = welcome
plugin_welcome.depends = plugin_coreplugin
SUBDIRS += plugin_welcome
}

# RawHID connection plugin
SUBDIRS += plugin_rawhid
plugin_rawhid.subdir = rawhid
plugin_rawhid.depends = plugin_coreplugin

!CONFIG(SLIM_GCS) {
# Serial port connection plugin
SUBDIRS += plugin_serial
plugin_serial.subdir = serialconnection
plugin_serial.depends = plugin_coreplugin
}

# UAVObjects plugin
SUBDIRS += plugin_uavobjects
plugin_uavobjects.subdir = uavobjects
plugin_uavobjects.depends = plugin_coreplugin

# UAVTalk plugin
SUBDIRS += plugin_uavtalk
plugin_uavtalk.subdir = uavtalk
plugin_uavtalk.depends = plugin_uavobjects
plugin_uavtalk.depends += plugin_coreplugin

!CONFIG(SLIM_GCS) {
# UAVTalkRelay plugin
SUBDIRS += plugin_uavtalkrelay
plugin_uavtalkrelay.subdir = uavtalkrelay
plugin_uavtalkrelay.depends = plugin_uavobjects
plugin_uavtalkrelay.depends += plugin_coreplugin
plugin_uavtalkrelay.depends += plugin_uavtalk
}

!CONFIG(SLIM_GCS) {
# OPMap UAVGadget
!LIGHTWEIGHT_GCS {
plugin_opmap.subdir = opmap
plugin_opmap.depends = plugin_coreplugin
plugin_opmap.depends += plugin_uavobjects
plugin_opmap.depends += plugin_uavobjectutil
plugin_opmap.depends += plugin_uavtalk
plugin_opmap.depends += plugin_pathplanner
SUBDIRS += plugin_opmap
}
}

!CONFIG(SLIM_GCS) {
# Scope UAVGadget
plugin_scope.subdir = scope
plugin_scope.depends = plugin_coreplugin
plugin_scope.depends += plugin_uavobjects
plugin_scope.depends += plugin_uavtalk
SUBDIRS += plugin_scope
}

!CONFIG(SLIM_GCS) {
# UAVObject Browser gadget
plugin_uavobjectbrowser.subdir = uavobjectbrowser
plugin_uavobjectbrowser.depends = plugin_coreplugin
plugin_uavobjectbrowser.depends += plugin_uavobjects
SUBDIRS += plugin_uavobjectbrowser
}

!CONFIG(SLIM_GCS) {
# ModelView UAVGadget
!LIGHTWEIGHT_GCS {
plugin_modelview.subdir = modelview
plugin_modelview.depends = plugin_coreplugin
plugin_modelview.depends += plugin_uavobjects
SUBDIRS += plugin_modelview
}
}

!CONFIG(SLIM_GCS) {
# Notify gadget NEEDS PHONON UPGRADED TO QT5
#!disable_notify_plugin {
#    plugin_notify.subdir = notify
#    plugin_notify.depends = plugin_coreplugin
#    plugin_notify.depends += plugin_uavobjects
#    plugin_notify.depends += plugin_uavtalk
#    SUBDIRS += plugin_notify
#}
}

# Uploader gadget
plugin_uploader.subdir = uploader
plugin_uploader.depends = plugin_coreplugin
plugin_uploader.depends += plugin_uavobjects
plugin_uploader.depends += plugin_uavtalk
plugin_uploader.depends += plugin_rawhid
plugin_uploader.depends += plugin_uavobjectutil
SUBDIRS += plugin_uploader

!CONFIG(SLIM_GCS) {
# Dial gadget
plugin_dial.subdir = dial
plugin_dial.depends = plugin_coreplugin
plugin_dial.depends += plugin_uavobjects
SUBDIRS += plugin_dial
}

!CONFIG(SLIM_GCS) {
# Linear Dial gadget
plugin_lineardial.subdir = lineardial
plugin_lineardial.depends = plugin_coreplugin
plugin_lineardial.depends += plugin_uavobjects
SUBDIRS += plugin_lineardial
}

!CONFIG(SLIM_GCS) {
# System Health gadget
plugin_systemhealth.subdir = systemhealth
plugin_systemhealth.depends = plugin_coreplugin
plugin_systemhealth.depends += plugin_uavobjects
plugin_systemhealth.depends += plugin_uavtalk
SUBDIRS += plugin_systemhealth
}

!CONFIG(SLIM_GCS) {
# Config gadget
plugin_config.subdir = config
plugin_config.depends = plugin_coreplugin
plugin_config.depends += plugin_uavtalk
plugin_config.depends += plugin_uavobjects
plugin_config.depends += plugin_uavobjectutil
plugin_config.depends += plugin_uavobjectwidgetutils
plugin_config.depends += plugin_uavsettingsimportexport
SUBDIRS += plugin_config

# GPS Display gadget
plugin_gpsdisplay.subdir = gpsdisplay
plugin_gpsdisplay.depends = plugin_coreplugin
plugin_gpsdisplay.depends += plugin_uavobjects
SUBDIRS += plugin_gpsdisplay

# QML viewer gadget
!LIGHTWEIGHT_GCS {
plugin_qmlview.subdir = qmlview
plugin_qmlview.depends = plugin_coreplugin
plugin_qmlview.depends += plugin_uavobjects
SUBDIRS += plugin_qmlview
}

# Path Planner gadget
plugin_pathplanner.subdir = pathplanner
plugin_pathplanner.depends = plugin_coreplugin
plugin_pathplanner.depends += plugin_uavobjects
SUBDIRS += plugin_pathplanner

# PicoC gadget
plugin_picoc.subdir = picoc
plugin_picoc.depends = plugin_coreplugin
plugin_picoc.depends += plugin_uavobjects
SUBDIRS += plugin_picoc

# Telemetry Scheduler gadget
plugin_telemetryscheduler.subdir = telemetryscheduler
plugin_telemetryscheduler.depends = plugin_coreplugin
plugin_telemetryscheduler.depends += plugin_uavobjects
plugin_telemetryscheduler.depends += plugin_uavobjectutil
SUBDIRS += plugin_telemetryscheduler

# Primary Flight Display (PFD) gadget, QML version
!LIGHTWEIGHT_GCS {
plugin_pfdqml.subdir = pfdqml
plugin_pfdqml.depends = plugin_coreplugin
plugin_pfdqml.depends += plugin_uavobjects
SUBDIRS += plugin_pfdqml
}

CONFIG(SLIM_GCS) {
# IP connection plugin
plugin_ipconnection.subdir = ipconnection
plugin_ipconnection.depends = plugin_coreplugin
SUBDIRS += plugin_ipconnection
}

#HITL Simulation gadget
!LIGHTWEIGHT_GCS {
plugin_hitl.subdir = hitl
plugin_hitl.depends = plugin_coreplugin
plugin_hitl.depends += plugin_uavobjects
plugin_hitl.depends += plugin_uavtalk
SUBDIRS += plugin_hitl
}

# Export and Import GCS Configuration
plugin_importexport.subdir = importexport
plugin_importexport.depends = plugin_coreplugin
SUBDIRS += plugin_importexport

# Telemetry data logging plugin
plugin_logging.subdir = logging
plugin_logging.depends = plugin_coreplugin
plugin_logging.depends += plugin_uavobjects
plugin_logging.depends += plugin_uavtalk
plugin_logging.depends += plugin_scope
SUBDIRS += plugin_logging

# TauLink monitoring plugin
plugin_taulink.subdir = taulink
plugin_taulink.depends = plugin_coreplugin
plugin_taulink.depends += plugin_uavobjects
plugin_taulink.depends += plugin_uavtalk
plugin_taulink.depends += plugin_uavobjectwidgetutils
SUBDIRS += plugin_taulink

KML {
    # KML Export plugin
    plugin_kmlexport.subdir = kmlexport
    plugin_kmlexport.depends = plugin_coreplugin
    plugin_kmlexport.depends += plugin_uavobjects
    plugin_kmlexport.depends += plugin_uavtalk
    SUBDIRS += plugin_kmlexport
}

# GCS Control of UAV gadget
!LIGHTWEIGHT_GCS {
    # GCS Control plugin
    plugin_gcscontrolplugin.subdir = gcscontrolplugin
    plugin_gcscontrolplugin.depends = plugin_coreplugin
    plugin_gcscontrolplugin.depends += plugin_uavobjects
    SUBDIRS += plugin_gcscontrolplugin
}

# UAV Object Utility plugin
plugin_uavobjectutil.subdir = uavobjectutil
plugin_uavobjectutil.depends = plugin_coreplugin
plugin_uavobjectutil.depends += plugin_uavobjects
SUBDIRS += plugin_uavobjectutil

!CONFIG(SLIM_GCS) {
# OSG Earth View plugin
OSG {
    plugin_osgearthview.subdir = osgearthview
    plugin_osgearthview.depends = plugin_coreplugin
    plugin_osgearthview.depends += plugin_uavobjects
    plugin_osgearthview.depends += plugin_uavobjectwidgetutils
    SUBDIRS += plugin_osgearthview
}

# Magic Waypoint gadget
!LIGHTWEIGHT_GCS {
plugin_magicwaypoint.subdir = magicwaypoint
plugin_magicwaypoint.depends = plugin_coreplugin
plugin_magicwaypoint.depends = plugin_uavobjects
SUBDIRS += plugin_magicwaypoint
}
}

# UAV Settings Import/Export plugin
plugin_uavsettingsimportexport.subdir = uavsettingsimportexport
plugin_uavsettingsimportexport.depends = plugin_coreplugin
plugin_uavsettingsimportexport.depends += plugin_uavobjects
plugin_uavsettingsimportexport.depends += plugin_uavobjectutil
SUBDIRS += plugin_uavsettingsimportexport

# UAV Object Widget Utility plugin
plugin_uavobjectwidgetutils.subdir = uavobjectwidgetutils
plugin_uavobjectwidgetutils.depends = plugin_coreplugin
plugin_uavobjectwidgetutils.depends += plugin_uavobjects
plugin_uavobjectwidgetutils.depends += plugin_uavobjectutil
plugin_uavobjectwidgetutils.depends += plugin_uavsettingsimportexport
plugin_uavobjectwidgetutils.depends += plugin_uavtalk
SUBDIRS += plugin_uavobjectwidgetutils

!CONFIG(SLIM_GCS) {
# Setup Wizard plugin
plugin_setupwizard.subdir = setupwizard
plugin_setupwizard.depends = plugin_coreplugin
plugin_setupwizard.depends += plugin_uavobjectutil
plugin_setupwizard.depends += plugin_config
plugin_setupwizard.depends += plugin_uploader
SUBDIRS += plugin_setupwizard

# RFM22b Wizard plugin
plugin_rfmbindwizard.subdir = rfmbindwizard
plugin_rfmbindwizard.depends = plugin_coreplugin
plugin_rfmbindwizard.depends += plugin_uavobjectutil
plugin_rfmbindwizard.depends += plugin_config
plugin_rfmbindwizard.depends += plugin_uploader
SUBDIRS += plugin_rfmbindwizard

# Setup alarm messaging plugin
plugin_sysalarmsmessaging.subdir = sysalarmsmessaging
plugin_sysalarmsmessaging.depends = plugin_coreplugin
plugin_sysalarmsmessaging.depends += plugin_uavobjects
plugin_sysalarmsmessaging.depends += plugin_uavtalk
SUBDIRS += plugin_sysalarmsmessaging
}

############################
#  Board plugins
# Those plugins define supported board models: each board manufacturer
# needs to implement a manufacturer plugin that defines all their boards
############################

# Tau Labs project
plugin_boards_taulabs.subdir = boards_taulabs
plugin_boards_taulabs.depends += plugin_coreplugin
plugin_boards_taulabs.depends += plugin_uavobjects
plugin_boards_taulabs.depends += plugin_uavobjectutil
plugin_boards_taulabs.depends += plugin_uavobjectwidgetutils
SUBDIRS += plugin_boards_taulabs

# OpenPilot project
plugin_boards_openpilot.subdir = boards_openpilot
plugin_boards_openpilot.depends = plugin_coreplugin
plugin_boards_openpilot.depends = plugin_uavobjects
plugin_boards_openpilot.depends = plugin_uavobjectutil
plugin_boards_openpilot.depends += plugin_uavobjectwidgetutils

SUBDIRS += plugin_boards_openpilot

# Quantec Networks GmbH
plugin_boards_quantec.subdir = boards_quantec
plugin_boards_quantec.depends = plugin_coreplugin
plugin_boards_quantec.depends = plugin_uavobjects
SUBDIRS += plugin_boards_quantec

# Naze32
plugin_boards_naze.subdir = boards_naze
plugin_boards_naze.depends = plugin_coreplugin
plugin_boards_naze.depends = plugin_uavobjects
SUBDIRS += plugin_boards_naze

# Team Black Sheep
plugin_boards_tbs.subdir = boards_tbs
plugin_boards_tbs.depends = plugin_coreplugin
plugin_boards_tbs.depends = plugin_uavobjects
plugin_boards_tbs.depends = plugin_uavobjectutil
plugin_boards_tbs.depends += plugin_uavobjectwidgetutils
SUBDIRS += plugin_boards_tbs

# STM boards
plugin_boards_stm.subdir = boards_stm
plugin_boards_stm.depends = plugin_coreplugin
plugin_boards_stm.depends = plugin_uavobjects
SUBDIRS += plugin_boards_stm

# AeroQuad AQ32
plugin_boards_aeroquad.subdir = boards_aeroquad
plugin_boards_aeroquad.depends = plugin_coreplugin
plugin_boards_aeroquad.depends = plugin_uavobjects
SUBDIRS += plugin_boards_aeroquad

CONFIG(SLIM_GCS) {
# SlimConfig gadget
plugin_slimconfig.subdir = slimconfig
plugin_slimconfig.depends = plugin_coreplugin
plugin_slimconfig.depends += plugin_uavtalk
plugin_slimconfig.depends += plugin_uavobjects
plugin_slimconfig.depends += plugin_uavobjectutil
plugin_slimconfig.depends += plugin_uavobjectwidgetutils
plugin_slimconfig.depends += plugin_uavsettingsimportexport
SUBDIRS += plugin_slimconfig

# Updater Plugin
plugin_updater.subdir = updater
plugin_updater.depends = plugin_coreplugin
plugin_updater.depends += plugin_uavtalk
plugin_updater.depends += plugin_uavobjectutil
SUBDIRS += plugin_updater
}
SLIM_GCS {
INCLUDEPATH+=./slimcoreplugin
}
!SLIM_GCS {
INCLUDEPATH+=./coreplugin
}
