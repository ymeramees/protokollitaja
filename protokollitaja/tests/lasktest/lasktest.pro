QT += testlib
QT -= gui

INCLUDEPATH += ../../main/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_lask.cpp \
    ../../main/lask.cpp

HEADERS += ../../main/lask.h
