QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on depend_includepath testcase

INCLUDEPATH += ../../protofinaal/ \

TEMPLATE = app

SOURCES +=  tst_initialdialogtest.cpp \
    ../../protofinaal/initialdialog.cpp

HEADERS += \
    ../../protofinaal/initialdialog.h

FORMS += \
    ../../protofinaal/initialdialog.ui
