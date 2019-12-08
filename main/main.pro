#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T16:24:54
#
#-------------------------------------------------
INCLUDEPATH += ./ \
INCLUDEPATH += ../../protokollitaja/main
include(qextserialport/src/qextserialport.pri)

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protolehelugeja
TEMPLATE = app


SOURCES += main.cpp \
    lehelugeja.cpp \
    logandcmdwindow.cpp \
    scoringmachineconnection.cpp \
    ../../protokollitaja/main/lask.cpp \
    ../../protokollitaja/main/common/target.cpp

HEADERS  += lehelugeja.h \
    logandcmdwindow.h \
    version.h \
    scoringmachineconnection.h \
    ../../protokollitaja/main/lask.h \
    ../../protokollitaja/main/common/target.h

FORMS    += lehelugeja.ui \
    logandcmdwindow.ui

RESOURCES += \
    ../protolehelugeja.qrc
RC_FILE = ikoon.rc
