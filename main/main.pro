#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T16:24:54
#
#-------------------------------------------------
INCLUDEPATH += ./ \
INCLUDEPATH += ../../protokollitaja/src
include(qextserialport/src/qextserialport.pri)

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protolehelugeja
TEMPLATE = app


SOURCES += main.cpp \
    lehelugeja.cpp \
    scoringmachineconnection.cpp \
    ../../protokollitaja/src/lask.cpp \
    ../../protokollitaja/src/common/target.cpp

HEADERS  += lehelugeja.h \
    version.h \
    scoringmachineconnection.h \
    ../../protokollitaja/src/lask.h \
    ../../protokollitaja/src/common/target.h

FORMS    += lehelugeja.ui

RESOURCES += \
    ../protolehelugeja.qrc
RC_FILE = ikoon.rc
