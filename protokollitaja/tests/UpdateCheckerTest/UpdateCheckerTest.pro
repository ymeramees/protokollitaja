QT += testlib network
QT -= gui

INCLUDEPATH += ../../protokollitaja/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  ../../protokollitaja/filedownloader.cpp \
    ../../protokollitaja/updatechecker.cpp \
    tst_updatechecker.cpp

HEADERS += ../../protokollitaja/filedownloader.h \
    ../../protokollitaja/updatechecker.h
