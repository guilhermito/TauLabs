INCLUDEPATH+=./slimcoreplugin

plugin_coreplugin.subdir = slimcoreplugin

# SlimConfig gadget
plugin_slimconfig.subdir = slimconfig
plugin_slimconfig.depends = plugin_coreplugin
plugin_slimconfig.depends += plugin_uavtalk
plugin_slimconfig.depends += plugin_uavobjects
plugin_slimconfig.depends += plugin_uavobjectutil
plugin_slimconfig.depends += plugin_uavobjectwidgetutils
plugin_slimconfig.depends += plugin_uavsettingsimportexport
SUBDIRS += plugin_slimconfig