TEMPLATE = app
TARGET = Protokollitaja
INCLUDEPATH += ./ \
INCLUDEPATH += ../../qfinaal/src
DEPENDPATH += $${INCLUDEPATH}

include(qextserialport/src/qextserialport.pri)
include(xlslib/xlslib.pri)

QT += core \
    gui \
    sql \
    network \
    printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += lisalaskudeaken.h \
    globalvariables.h \
    liikmetevalikkast.h \
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
    lask.h \
    version.h \
    laskudeaken.h \
    ../../qfinaal/src/uhendumiseaken.h \
    filedownloader.h \
    finalsfileexport.h \
    startlistwriter.h \
    protolehelugejaserver.h \
    protolehelugejaconnection.h \
    siusdataconnections.h \
    siusdataconnection.h
SOURCES += lisalaskudeaken.cpp \
    liikmetevalikkast.cpp \
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
    lask.cpp \
    laskudeaken.cpp \
    ../../qfinaal/src/uhendumiseaken.cpp \
    filedownloader.cpp \
    finalsfileexport.cpp \
    startlistwriter.cpp \
    protolehelugejaserver.cpp \
    protolehelugejaconnection.cpp \
    siusdataconnections.cpp \
    siusdataconnection.cpp
FORMS += lisalaskudeaken.ui \
    liikmetevalikkast.ui \
    importaken.ui \
    algusevalik.ui \
    seadedkast.ui \
    valikkast.ui \
    lehelugejaaken.ui \
    sifriseade.ui \
    laskudeaken.ui \
    ../../qfinaal/src/uhendumiseaken.ui \
    finalsfileexport.ui \
    siusdataconnections.ui
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
