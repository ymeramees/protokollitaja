INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/connectionserver.cpp \
    $$PWD/dataconnection.cpp \
    $$PWD/header.cpp \
    $$PWD/inbandconnection.cpp \
    $$PWD/lane.cpp \
    $$PWD/../../common/src/lask.cpp \
    $$PWD/../../common/src/qualificationevents.cpp \
    $$PWD/rangecontrol.cpp

HEADERS += \
    $$PWD/connectionserver.h \
    $$PWD/dataconnection.h \
    $$PWD/header.h \
    $$PWD/inbandconnection.h \
    $$PWD/lane.h \
    $$PWD/rangecontrol.h \
    $$PWD/programname.h \
    $$PWD/../../common/src/version.h \
    $$PWD/../../common/src/lask.h \
    $$PWD/../../common/src/siusshotdata.h \
    $$PWD/../../common/src/qualificationevents.h

