QT += testlib
QT += gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../main/ \
    ../../../protokollitaja/main
include(../../main/main.pri)

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp
