QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../../protokollitaja/

CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

SOURCES +=  tst_initialdialogtest.cpp \
    ../../protokollitaja/initialdialog.cpp \
    ../../protokollitaja/simplekllfilerw.cpp

HEADERS += \
    ../../protokollitaja/initialdialog.h \
    ../../protokollitaja/ui_initialdialog.h \
    ../../protokollitaja/simplekllfilerw.h

FORMS += \
    ../../protokollitaja/initialdialog.ui
