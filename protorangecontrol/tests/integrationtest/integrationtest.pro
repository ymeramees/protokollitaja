QT += testlib
QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protorangecontrol/ \
    ../../../common/src
DEPENDPATH += $${INCLUDEPATH}
include(../../protorangecontrol/protorangecontrol.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_integrationtest.cpp
