QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/ \
    ../../../common/src/

CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES +=  tst_startlistwritertest.cpp \
    ../../protokollitaja/startlistwriter.cpp \
    ../../../common/src/commonsettings.cpp

HEADERS += \
    ../../protokollitaja/startlistwriter.h \
    ../../../common/src/commonsettings.h
