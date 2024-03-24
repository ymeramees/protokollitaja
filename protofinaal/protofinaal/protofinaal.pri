INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/protofinaal.cpp \
    $$PWD/shotedit.cpp \
    $$PWD/team.cpp \
    $$PWD/competitor.cpp \
    $$PWD/initialdialog.cpp \
    $$PWD/spectatorwindow.cpp \
    $$PWD/teamstable.cpp \
    $$PWD/../../common/src/commonsettings.cpp \
    $$PWD/../../common/src/siusdataconnection.cpp \
    $$PWD/../../common/src/siusdataconnections.cpp \
    $$PWD/../../common/src/lask.cpp \
    $$PWD/../../common/src/finalsformats.cpp

HEADERS += $$PWD/protofinaal.h \
    $$PWD/shotedit.h \
    $$PWD/team.h \
    $$PWD/competitor.h \
    $$PWD/initialdialog.h \
    $$PWD/spectatorwindow.h \
    $$PWD/teamstable.h \
    $$PWD/../../common/src/commonsettings.h \
    $$PWD/../../common/src/siusdataconnection.h \
    $$PWD/../../common/src/siusdataconnections.h \
    $$PWD/../../common/src/lask.h \
    $$PWD/../../common/src/finalsformats.h

FORMS += $$PWD/initialdialog.ui \
    $$PWD/../../common/src/siusdataconnections.ui

RESOURCES += \
    $$PWD/../protofinaal.qrc
