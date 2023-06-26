QT += testlib
QT -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/
INCLUDEPATH += ../../../common/src/

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_sheet.cpp \
    ../../protokollitaja/laskur.cpp \
    ../../../common/src/lask.cpp \
    ../../protokollitaja/lisalaskudeaken.cpp \
    ../../protokollitaja/liikmetevalikkast.cpp \
    ../../protokollitaja/laskudeaken.cpp \
    ../../protokollitaja/leht.cpp \
    ../../protokollitaja/voistkond.cpp

HEADERS += ../../protokollitaja/laskur.h \
    ../../../common/src/lask.h \
    ../../protokollitaja/lisalaskudeaken.h \
    ../../protokollitaja/liikmetevalikkast.h \
    ../../protokollitaja/laskudeaken.h \
    ../../protokollitaja/leht.h \
    ../../protokollitaja/voistkond.h

FORMS += ../../protokollitaja/lisalaskudeaken.ui \
    ../../protokollitaja/liikmetevalikkast.ui \
    ../../protokollitaja/laskudeaken.ui
