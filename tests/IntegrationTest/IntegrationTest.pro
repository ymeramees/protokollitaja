QT += testlib
QT += gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../main/ \
    ../../../protokollitaja/main
include(../../main/main.pri)
#include(../../main/qextserialport/src/qextserialport.pri)

CONFIG += qt warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_integrationtest.cpp
