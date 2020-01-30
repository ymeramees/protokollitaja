QT += testlib
QT += gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../../protokollitaja/main \
    ../../main/

CONFIG += qt warn_on testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../../main/scoringmachineconnection.cpp \
    ../../../protokollitaja/main/lask.cpp

HEADERS += ../../main/scoringmachineconnection.h \
    ../../../protokollitaja/main/lask.h
