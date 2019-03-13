QT += testlib
QT += gui
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES += *.cpp \
    ../src/scoringmachineconnection.cpp

HEADERS += *.h \
    ../src/scoringmachineconnection.h
