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
    ../../../common/src/lask.cpp \
    ../../protokollitaja/lisalaskudeaken.cpp \
    ../../protokollitaja/laskudeaken.cpp

HEADERS += ../../protokollitaja/laskur.h \
    ../../../common/src/lask.h \
    ../../protokollitaja/lisalaskudeaken.h \
    ../../protokollitaja/laskudeaken.h

FORMS += ../../protokollitaja/lisalaskudeaken.ui \
    ../../protokollitaja/laskudeaken.ui
