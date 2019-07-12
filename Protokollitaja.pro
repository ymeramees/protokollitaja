TEMPLATE = app
TARGET = Protokollitaja
INCLUDEPATH += ./src \
DEPENDPATH += $${INCLUDEPATH}

include(src/qextserialport/src/qextserialport.pri)
include(src/xlslib/xlslib.pri)

QT += core \
    gui \
    sql \
    network \
    printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += src/lisalaskudeaken.h \
    src/liikmetevalikkast.h \
    src/voistkond.h \
    src/importaken.h \
    src/tulemuseaken.h \
    src/algusevalik.h \
    src/seadedkast.h \
    src/valikkast.h \
    src/andmebaas.h \
    src/leht.h \
    src/laskur.h \
    src/protokollitaja.h \
    src/lehelugejaaken.h \
    src/sifriseade.h \
    src/lask.h \
    src/version.h \
    src/laskudeaken.h \
    ../qfinaal/src/uhendumiseaken.h \
    src/filedownloader.h \
    src/finalsfileexport.h \
    src/startlistwriter.h \
    src/protolehelugejaserver.h \
    src/protolehelugejaconnection.h \
    src/siusdataconnections.h \
    src/siusdataconnection.h
SOURCES += src/lisalaskudeaken.cpp \
    src/liikmetevalikkast.cpp \
    src/voistkond.cpp \
    src/importaken.cpp \
    src/tulemuseaken.cpp \
    src/algusevalik.cpp \
    src/seadedkast.cpp \
    src/valikkast.cpp \
    src/leht.cpp \
    src/laskur.cpp \
    src/main.cpp \
    src/protokollitaja.cpp \
    src/lehelugejaaken.cpp \
    src/sifriseade.cpp \
    src/lask.cpp \
    src/laskudeaken.cpp \
    ../qfinaal/src/uhendumiseaken.cpp \
    src/filedownloader.cpp \
    src/finalsfileexport.cpp \
    src/startlistwriter.cpp \
    src/protolehelugejaserver.cpp \
    src/protolehelugejaconnection.cpp \
    src/siusdataconnections.cpp \
    src/siusdataconnection.cpp
FORMS += src/lisalaskudeaken.ui \
    src/liikmetevalikkast.ui \
    src/importaken.ui \
    src/algusevalik.ui \
    src/seadedkast.ui \
    src/valikkast.ui \
    src/lehelugejaaken.ui \
    src/sifriseade.ui \
    src/laskudeaken.ui \
    ../qfinaal/src/uhendumiseaken.ui \
    src/finalsfileexport.ui \
    src/siusdataconnections.ui
RESOURCES += protokollitaja.qrc
RC_FILE += src/ikoon.rc
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
