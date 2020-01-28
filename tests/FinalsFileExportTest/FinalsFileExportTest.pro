QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../main/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_finalsfileexport.cpp \
    ../../main/finalsfileexport.cpp \
    ../../main/startlistwriter.cpp

HEADERS += ../../main/finalsfileexport.h \
    ../../main/startlistwriter.h

FORMS += ../../main/finalsfileexport.ui
