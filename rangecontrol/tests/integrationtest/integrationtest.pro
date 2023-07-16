QT += testlib
QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../rangecontrol/ \
    ../../../common/src
DEPENDPATH += $${INCLUDEPATH}
include(../../rangecontrol/rangecontrol.pri)

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_integrationtest.cpp
