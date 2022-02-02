#-------------------------------------------------
#
# Project initially created sometime in 2007-2008
#
#-------------------------------------------------
TEMPLATE = subdirs

CONFIG += c++17

SUBDIRS += protokollitaja \
    protolehelugeja \
    protofinaal \
    common

HEADERS += \
    common/src/competitorshot.h

SOURCES += \
    common/src/competitorshot.cpp
