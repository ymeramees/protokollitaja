#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T16:24:54
#
#-------------------------------------------------
include(qextserialport/src/qextserialport.pri)

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protolehelugeja
TEMPLATE = app


SOURCES += main.cpp \
        lehelugeja.cpp \
    ../../protokollitaja/src/lask.cpp \
    scoringmachineconnection.cpp

HEADERS  += lehelugeja.h \
    ../../protokollitaja/src/lask.h \
    version.h \
    scoringmachineconnection.h

FORMS    += lehelugeja.ui

RESOURCES += \
    ../protolehelugeja.qrc
RC_FILE = ikoon.rc
