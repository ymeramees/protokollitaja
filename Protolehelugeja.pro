#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T16:24:54
#
#-------------------------------------------------
include(src/qextserialport/src/qextserialport.pri)

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Protolehelugeja
TEMPLATE = app


SOURCES += src/main.cpp\
        src/lehelugeja.cpp \
    ../protokollitaja/src/lask.cpp

HEADERS  += src/lehelugeja.h \
    ../protokollitaja/src/lask.h \
    src/version.h

FORMS    += src/lehelugeja.ui

RESOURCES += \
    protolehelugeja.qrc
RC_FILE = src/ikoon.rc
