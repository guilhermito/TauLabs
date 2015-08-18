# USE .subdir AND .depends !
# OTHERWISE PLUGINS WILL BUILD IN WRONG ORDER (DIRECTORIES ARE COMPILED IN PARALLEL)

TEMPLATE  = subdirs

SUBDIRS   = plugin_coreplugin

CONFIG(SLIM_GCS) {
    include(slim_gcs.pri)
} else {
    include(full_gcs.pri)
}

# Blank Template Plugin, not compiled by default
#SUBDIRS += plugin_donothing
#plugin_donothing.subdir = donothing
#plugin_donothing.depends = plugin_coreplugin

 # UAVObjects plugin
SUBDIRS += plugin_uavobjects
plugin_uavobjects.subdir = uavobjects
plugin_uavobjects.depends = plugin_coreplugin

# UAVTalk plugin
SUBDIRS += plugin_uavtalk
plugin_uavtalk.subdir = uavtalk
plugin_uavtalk.depends = plugin_uavobjects
plugin_uavtalk.depends += plugin_coreplugin

# RawHID connection plugin
SUBDIRS += plugin_rawhid
plugin_rawhid.subdir = rawhid
plugin_rawhid.depends = plugin_coreplugin

# Uploader gadget
plugin_uploader.subdir = uploader
plugin_uploader.depends = plugin_coreplugin
plugin_uploader.depends += plugin_uavobjects
plugin_uploader.depends += plugin_uavtalk
plugin_uploader.depends += plugin_rawhid
plugin_uploader.depends += plugin_uavobjectutil
SUBDIRS += plugin_uploader

# UAV Object Utility plugin
plugin_uavobjectutil.subdir = uavobjectutil
plugin_uavobjectutil.depends = plugin_coreplugin
plugin_uavobjectutil.depends += plugin_uavobjects
SUBDIRS += plugin_uavobjectutil

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
