QT += testlib
QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../src

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../../src/siusdataconnection.cpp \
    ../../src/lask.cpp \
    ../../src/commonsettings.cpp

HEADERS += ../../src/siusdataconnection.h \
    ../../src/lask.h \
    ../../src/commonsettings.h
