QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

INCLUDEPATH += ../../src

TEMPLATE = app

SOURCES +=  \
    tst_commonsettings.cpp \
    ../../src/commonsettings.cpp

HEADERS += ../../src/commonsettings.h
