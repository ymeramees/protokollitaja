QT += testlib
QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on depend_includepath testcase

INCLUDEPATH += ../../protofinaal \
INCLUDEPATH += ../../../common/src/
include(../../protofinaal/protofinaal.pri)

TEMPLATE = app

SOURCES +=  tst_protofinaaltest.cpp
