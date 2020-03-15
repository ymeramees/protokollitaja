QT += testlib
QT += gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../../../protokollitaja/main \
    ../../protolehelugeja/

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../../protolehelugeja/scoringmachineconnection.cpp \
    ../../../../protokollitaja/main/lask.cpp

HEADERS += ../../protolehelugeja/scoringmachineconnection.h \
    ../../../../protokollitaja/main/lask.h
