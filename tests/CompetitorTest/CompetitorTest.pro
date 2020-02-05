QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../main/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_competitor.cpp \
    ../../main/laskur.cpp \
    ../../main/lask.cpp \
    ../../main/lisalaskudeaken.cpp \
    ../../main/laskudeaken.cpp

HEADERS += ../../main/laskur.h \
    ../../main/lask.h \
    ../../main/lisalaskudeaken.h \
    ../../main/laskudeaken.h

FORMS += ../../main/lisalaskudeaken.ui \
    ../../main/laskudeaken.ui
