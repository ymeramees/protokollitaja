QT += testlib
QT += gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../../common/src \
    ../../protolehelugeja/

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../../protolehelugeja/scoringmachineconnection.cpp \
    ../../../common/src/lask.cpp

HEADERS += ../../protolehelugeja/scoringmachineconnection.h \
    ../../../common/src/lask.h
