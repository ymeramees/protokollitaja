QT += testlib
QT += gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../../protokollitaja/siusdataconnection.cpp

HEADERS += ../../protokollitaja/siusdataconnection.h
