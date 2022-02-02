TEMPLATE = app
TARGET = Protokollitaja
INCLUDEPATH += ./ \
INCLUDEPATH += ../../common/src
DEPENDPATH += $${INCLUDEPATH}

include(../../common/src/qextserialport/src/qextserialport.pri)
include(xlslib/xlslib.pri)
if ( !include( ../../common/src/asmCrashReport/asmCrashReport.pri ) ) {
    error( Could not find the asmCrashReport.pri file. )
}

QT += core \
    gui \
    sql \
    network \
    printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += lisalaskudeaken.h \
    ../../common/src/competitorshot.h \
    competitionsettings.h \
    datauploader.h \
    initialdialog.h \
    kllfilerw.h \
    liikmetevalikkast.h \
    shotstablemodel.h \
    shotswindow.h \
    simplekllfilerw.h \
    tabwidgetwithsettings.h \
    updatechecker.h \
    voistkond.h \
    importaken.h \
    tulemuseaken.h \
    seadedkast.h \
    valikkast.h \
    andmebaas.h \
    leht.h \
    laskur.h \
    protokollitaja.h \
    lehelugejaaken.h \
    sifriseade.h \
    version.h \
    filedownloader.h \
    finalsfileexport.h \
    startlistwriter.h \
    protolehelugejaserver.h \
    protolehelugejaconnection.h \
    ../../common/src/commonsettings.h \
    ../../common/src/globalvariables.h \
    ../../common/src/lask.h \
    ../../common/src/uhendumiseaken.h \
    ../../common/src/siusdataconnections.h \
    ../../common/src/siusdataconnection.h \
    ../../common/src/siusshotdata.h
SOURCES += lisalaskudeaken.cpp \
    ../../common/src/competitorshot.cpp \
    datauploader.cpp \
    initialdialog.cpp \
    kllfilerw.cpp \
    liikmetevalikkast.cpp \
    shotstablemodel.cpp \
    shotswindow.cpp \
    simplekllfilerw.cpp \
    updatechecker.cpp \
    voistkond.cpp \
    importaken.cpp \
    tulemuseaken.cpp \
    seadedkast.cpp \
    valikkast.cpp \
    leht.cpp \
    laskur.cpp \
    main.cpp \
    protokollitaja.cpp \
    lehelugejaaken.cpp \
    sifriseade.cpp \
    filedownloader.cpp \
    finalsfileexport.cpp \
    startlistwriter.cpp \
    protolehelugejaserver.cpp \
    protolehelugejaconnection.cpp \
    ../../common/src/commonsettings.cpp \
    ../../common/src/lask.cpp \
    ../../common/src/uhendumiseaken.cpp \
    ../../common/src/siusdataconnections.cpp \
    ../../common/src/siusdataconnection.cpp
FORMS += lisalaskudeaken.ui \
    initialdialog.ui \
    liikmetevalikkast.ui \
    importaken.ui \
    seadedkast.ui \
    shotswindow.ui \
    valikkast.ui \
    lehelugejaaken.ui \
    sifriseade.ui \
    finalsfileexport.ui \
    ../../common/src/uhendumiseaken.ui \
    ../../common/src/siusdataconnections.ui
RESOURCES += ../protokollitaja.qrc
RC_FILE += ikoon.rc
win32{
INCLUDEPATH += $$(OPENSSL_INCLUDE_PATH)
LIBS += -liconv \
 -L $$(OPENSSL_LIB_PATH) -lcrypto -lssl
}
