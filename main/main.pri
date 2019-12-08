INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/lehelugeja.cpp \
    $$PWD/logandcmdwindow.cpp \
    $$PWD/scoringmachineconnection.cpp \
    $$PWD/../../protokollitaja/main/lask.cpp \
    $$PWD/../../protokollitaja/main/common/target.cpp

HEADERS  += $$PWD/lehelugeja.h \
    $$PWD/logandcmdwindow.h \
    $$PWD/version.h \
    $$PWD/scoringmachineconnection.h \
    $$PWD/../../protokollitaja/main/lask.h \
    $$PWD/../../protokollitaja/main/common/target.h

FORMS    += $$PWD/lehelugeja.ui \
    $$PWD/logandcmdwindow.ui

RESOURCES += \
    $$PWD/../protolehelugeja.qrc
RC_FILE = $$PWD/ikoon.rc
