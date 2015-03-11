include(../../plugins/uavobjects/uavobjects.pri)

SLIM_GCS {
include(../../plugins/slimcoreplugin/coreplugin.pri)
}
!SLIM_GCS {
include(../../plugins/coreplugin/coreplugin.pri)
}

include(../../libs/utils/utils.pri)
