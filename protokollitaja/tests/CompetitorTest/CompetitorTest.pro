QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/
INCLUDEPATH += ../../../common/src/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_competitor.cpp \
    ../../protokollitaja/laskur.cpp \
    ../../../common/src/competitorshot.cpp \
    ../../../common/src/lask.cpp \
    ../../protokollitaja/lisalaskudeaken.cpp \
    ../../protokollitaja/shotstablemodel.cpp \
    ../../protokollitaja/shotswindow.cpp

HEADERS += ../../protokollitaja/laskur.h \
    ../../../common/src/competitorshot.h \
    ../../../common/src/lask.h \
    ../../protokollitaja/lisalaskudeaken.h \
    ../../protokollitaja/shotstablemodel.h \
    ../../protokollitaja/shotswindow.h

FORMS += ../../protokollitaja/lisalaskudeaken.ui \
    ../../protokollitaja/shotswindow.ui
