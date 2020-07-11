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
    kllfilerw.h \
    liikmetevalikkast.h \
    tabwidgetwithsettings.h \
    updatechecker.h \
    voistkond.h \
    importaken.h \
    tulemuseaken.h \
    algusevalik.h \
    seadedkast.h \
    valikkast.h \
    andmebaas.h \
    leht.h \
    laskur.h \
    protokollitaja.h \
    lehelugejaaken.h \
    sifriseade.h \
    version.h \
    laskudeaken.h \
    filedownloader.h \
    finalsfileexport.h \
    startlistwriter.h \
    protolehelugejaserver.h \
    protolehelugejaconnection.h \
    ../../common/src/globalvariables.h \
    ../../common/src/lask.h \
    ../../common/src/uhendumiseaken.h \
    ../../common/src/siusdataconnections.h \
    ../../common/src/siusdataconnection.h
SOURCES += lisalaskudeaken.cpp \
    kllfilerw.cpp \
    liikmetevalikkast.cpp \
    updatechecker.cpp \
    voistkond.cpp \
    importaken.cpp \
    tulemuseaken.cpp \
    algusevalik.cpp \
    seadedkast.cpp \
    valikkast.cpp \
    leht.cpp \
    laskur.cpp \
    main.cpp \
    protokollitaja.cpp \
    lehelugejaaken.cpp \
    sifriseade.cpp \
    laskudeaken.cpp \
    filedownloader.cpp \
    finalsfileexport.cpp \
    startlistwriter.cpp \
    protolehelugejaserver.cpp \
    protolehelugejaconnection.cpp \
    ../../common/src/lask.cpp \
    ../../common/src/uhendumiseaken.cpp \
    ../../common/src/siusdataconnections.cpp \
    ../../common/src/siusdataconnection.cpp
FORMS += lisalaskudeaken.ui \
    liikmetevalikkast.ui \
    importaken.ui \
    algusevalik.ui \
    seadedkast.ui \
    valikkast.ui \
    lehelugejaaken.ui \
    sifriseade.ui \
    laskudeaken.ui \
    finalsfileexport.ui \
    ../../common/src/uhendumiseaken.ui \
    ../../common/src/siusdataconnections.ui
RESOURCES += ../protokollitaja.qrc
RC_FILE += ikoon.rc
win32{
INCLUDEPATH += D:\Qt\Tools\mingw730_32\opt\include
LIBS += -liconv \
 -L D:\Qt\Tools\mingw730_32\opt\lib -lcrypto #-L../libxl-2.2.0/bin -lxl \
    #-lpthread #\
    #-L../qextserialport/build -lqextserialport
}
#QMAKE_CXXFLAGS += -finput-charset=utf-8 \
    #-fexec-charset=utf-8
#QMAKE_LFLAGS += -lpthread \
#    -static-libstdc++ \
#    -static-libgcc
#unix{
#    QMAKE_LFLAGS += -LLIBDIR
#}
