QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/
INCLUDEPATH += ../../../common/src/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_shotswindow.cpp \
    ../../../common/src/competitorshot.cpp \
    ../../../common/src/lask.cpp \
    ../../protokollitaja/shotstablemodel.cpp \
    ../../protokollitaja/shotswindow.cpp

HEADERS += ../../../common/src/lask.h \
    ../../../common/src/competitorshot.h \
    ../../protokollitaja/shotstablemodel.h \
    ../../protokollitaja/shotswindow.h

FORMS += ../../protokollitaja/shotswindow.ui
