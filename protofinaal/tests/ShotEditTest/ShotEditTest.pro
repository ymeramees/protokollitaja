QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on depend_includepath testcase

INCLUDEPATH += ../../protofinaal/ \
    ../../../common/src/

TEMPLATE = app

SOURCES +=  tst_shotedittest.cpp \
    ../../../common/src/lask.cpp \
    ../../protofinaal/shotedit.cpp

HEADERS += \
    ../../../common/src/lask.h \
    ../../protofinaal/shotedit.h
