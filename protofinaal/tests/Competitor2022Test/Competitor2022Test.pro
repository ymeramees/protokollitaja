QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on depend_includepath testcase

INCLUDEPATH += ../../protofinaal \
INCLUDEPATH += ../../../common/src/

TEMPLATE = app

SOURCES +=  tst_competitor2022test.cpp \
    ../../protofinaal/competitor2022.cpp \
    ../../protofinaal/shotedit.cpp \
    ../../../common/src/lask.cpp

HEADERS += \
    ../../protofinaal/competitor2022.h \
    ../../protofinaal/shotedit.h \
    ../../../common/src/lask.h
