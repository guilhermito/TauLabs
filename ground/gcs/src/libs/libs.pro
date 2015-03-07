TEMPLATE  = subdirs
CONFIG   += ordered
CONFIG   += SLIM_GCS

QT += widgets

SUBDIRS   = \
    qscispinbox\
    qtconcurrent \
    aggregation \
    extensionsystem \
    utils \
    quazip

!CONFIG(SLIM_GCS) {
        SUBDIRS += tlmapcontrol
        SUBDIRS += qwt
        SDL {
            SUBDIRS += sdlgamepad
        }
}

!CONFIG(SLIM_GCS) {
    !LIGHTWEIGHT_GCS {
        SUBDIRS += glc_lib
    }
}
