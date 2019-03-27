QT += testlib
QT += gui serialport
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../src/scoringmachineconnection.cpp \
    ../../protokollitaja/src/lask.cpp

HEADERS += ../src/scoringmachineconnection.h \
    ../../protokollitaja/src/lask.h
