INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/lehelugeja.cpp \
    $$PWD/logandcmdwindow.cpp \
    $$PWD/scoringmachineconnection.cpp \
    $$PWD/../../protokollitaja/src/lask.cpp \
    $$PWD/../../protokollitaja/src/common/target.cpp

HEADERS  += $$PWD/lehelugeja.h \
    $$PWD/logandcmdwindow.h \
    $$PWD/version.h \
    $$PWD/scoringmachineconnection.h \
    $$PWD/../../protokollitaja/src/lask.h \
    $$PWD/../../protokollitaja/src/common/target.h

FORMS    += $$PWD/lehelugeja.ui \
    $$PWD/logandcmdwindow.ui

RESOURCES += \
    $$PWD/../protolehelugeja.qrc
RC_FILE = $$PWD/ikoon.rc
