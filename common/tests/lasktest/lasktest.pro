QT += testlib
QT -= gui

INCLUDEPATH += ../../src

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_lask.cpp \
    ../../src/lask.cpp

HEADERS += ../../src/lask.h
