QT += testlib
QT += gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protolehelugeja/ \
    ../../../common/src
include(../../protolehelugeja/protolehelugeja.pri)

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp
