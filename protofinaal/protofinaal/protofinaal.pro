#-------------------------------------------------
#
# Project created by QtCreator 2018-11-24T06:25:22
#
#-------------------------------------------------

INCLUDEPATH += ./ \
INCLUDEPATH += ../../common/src/

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protofinaal
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    competitor.cpp \
    main.cpp \
    protofinaal.cpp \
    shotedit.cpp \
    team.cpp \
    initialdialog.cpp \
    spectatorwindow.cpp \
    ../../common/src/commonsettings.cpp \
    ../../common/src/siusdataconnection.cpp \
    ../../common/src/siusdataconnections.cpp \
    ../../common/src/lask.cpp \
    teamstable.cpp

HEADERS += \
    competitor.h \
    protofinaal.h \
    shotedit.h \
    team.h \
    initialdialog.h \
    spectatorwindow.h \
    ../../common/src/commonsettings.h \
    ../../common/src/siusdataconnection.h \
    ../../common/src/siusdataconnections.h \
    ../../common/src/lask.h \
    teamstable.h

FORMS += \
    initialdialog.ui \
    ../../common/src/siusdataconnections.ui

RESOURCES += \
    ../protofinaal.qrc \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
