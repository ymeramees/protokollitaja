QT += testlib
QT -= gui

INCLUDEPATH += ../../protokollitaja/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_lask.cpp \
    ../../protokollitaja/lask.cpp

HEADERS += ../../protokollitaja/lask.h
