QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_kllfilerwtest.cpp \
    ../../protokollitaja/simplekllfilerw.cpp

HEADERS += \
    ../../protokollitaja/simplekllfilerw.h \
    ../../protokollitaja/competitionsettings.h
