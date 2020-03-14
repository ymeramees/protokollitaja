QT += testlib
QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../main/

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../../main/siusdataconnection.cpp

HEADERS += ../../main/siusdataconnection.h
