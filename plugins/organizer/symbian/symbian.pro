TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_symbian)
PLUGIN_TYPE=organizer
    
CONFIG += mobility
MOBILITY = organizer

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

symbian: { 
    load(data_caging_paths)

    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
#    TARGET.UID3 = 0x2002AC7B
  
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
  
    HEADERS += \
        qorganizersymbian_p.h
    SOURCES += \
        qorganizersymbian.cpp

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}