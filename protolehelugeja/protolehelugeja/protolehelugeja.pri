INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/lehelugeja.cpp \
    $$PWD/logandcmdwindow.cpp \
    $$PWD/scoringmachineconnection.cpp \
    $$PWD/../../common/src/lask.cpp \
    $$PWD/../../common/src/target.cpp \
    $$PWD/selftests.cpp

HEADERS  += $$PWD/lehelugeja.h \
    $$PWD/logandcmdwindow.h \
    $$PWD/version.h \
    $$PWD/scoringmachineconnection.h \
    $$PWD/../../common/src/lask.h \
    $$PWD/../../common/src/target.h \
    $$PWD/selftests.h

FORMS    += $$PWD/lehelugeja.ui \
    $$PWD/logandcmdwindow.ui

RESOURCES += \
    $$PWD/../protolehelugeja.qrc
RC_FILE = $$PWD/ikoon.rc
