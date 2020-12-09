QT += testlib network
QT -= gui

INCLUDEPATH += ../../protokollitaja/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_datauploadertest.cpp \
    ../../protokollitaja/datauploader.cpp

HEADERS += ../../protokollitaja/datauploader.h
