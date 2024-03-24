QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/ \
    ../../../common/src/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_finalsfileexport.cpp \
    ../../protokollitaja/finalsfileexport.cpp \
    ../../protokollitaja/startlistwriter.cpp \
    ../../../common/src/commonsettings.cpp \
    ../../../common/src/finalsformats.cpp

HEADERS += ../../protokollitaja/finalsfileexport.h \
    ../../protokollitaja/startlistwriter.h \
    ../../../common/src/commonsettings.h \
    ../../../common/src/finalsformats.h

FORMS += ../../protokollitaja/finalsfileexport.ui

RESOURCES += ../../protokollitaja.qrc
