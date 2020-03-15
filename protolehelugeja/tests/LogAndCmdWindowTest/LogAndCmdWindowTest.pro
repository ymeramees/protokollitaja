QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protolehelugeja/

CONFIG += qt console warn_on testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_logandcmdwindow.cpp \
    ../../protolehelugeja/logandcmdwindow.cpp

HEADERS += ../../protolehelugeja/logandcmdwindow.h

FORMS    += ../../protolehelugeja/logandcmdwindow.ui
