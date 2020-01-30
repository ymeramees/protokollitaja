QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../main/

CONFIG += qt console warn_on testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_logandcmdwindow.cpp \
    ../../main/logandcmdwindow.cpp

HEADERS += ../../main/logandcmdwindow.h

FORMS    += ../../main/logandcmdwindow.ui
