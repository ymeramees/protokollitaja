QT += testlib
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on depend_includepath testcase

INCLUDEPATH += ../../protofinaal/ \
    ../../../common/src/

TEMPLATE = app

SOURCES +=  tst_initialdialogtest.cpp \
    ../../protofinaal/initialdialog.cpp \
    ../../../common/src/finalsformats.cpp

HEADERS += \
    ../../protofinaal/initialdialog.h \
    ../../../common/src/finalsformats.h

FORMS += \
    ../../protofinaal/initialdialog.ui

RESOURCES += ../../protofinaal.qrc
