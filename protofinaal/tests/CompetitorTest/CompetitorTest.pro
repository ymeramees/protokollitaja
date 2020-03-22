QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on depend_includepath testcase

INCLUDEPATH += ../../protofinaal

TEMPLATE = app

SOURCES +=  tst_competitortest.cpp \
    ../../protofinaal/competitor.cpp

HEADERS += \
    ../../protofinaal/competitor.h
