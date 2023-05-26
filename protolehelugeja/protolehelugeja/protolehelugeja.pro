#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T16:24:54
#
#-------------------------------------------------
INCLUDEPATH += ./ \
INCLUDEPATH += ../../common/src

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protolehelugeja
TEMPLATE = app


SOURCES += main.cpp \
    lehelugeja.cpp \
    logandcmdwindow.cpp \
    scoringmachineconnection.cpp \
    ../../common/src/lask.cpp \
    ../../common/src/target.cpp \
    selftests.cpp

HEADERS  += lehelugeja.h \
    logandcmdwindow.h \
    selftests.h \
    version.h \
    scoringmachineconnection.h \
    ../../common/src/lask.h \
    ../../common/src/target.h

FORMS    += lehelugeja.ui \
    logandcmdwindow.ui

RESOURCES += \
    ../protolehelugeja.qrc
RC_FILE = ikoon.rc
