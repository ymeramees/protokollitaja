TEMPLATE = app
TARGET = ProtoRangeControl
INCLUDEPATH += ./ \
INCLUDEPATH += ../../common/src
DEPENDPATH += $${INCLUDEPATH}

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connectionserver.cpp \
    dataconnection.cpp \
    header.cpp \
    inbandconnection.cpp \
    lane.cpp \
    main.cpp \
    ../../common/src/lask.cpp \
    protorangecontrol.cpp

HEADERS += \
    connectionserver.h \
    dataconnection.h \
    header.h \
    inbandconnection.h \
    lane.h \
    protorangecontrol.h \
    programname.h \
    ../../common/src/version.h \
    ../../common/src/lask.h \
    ../../common/src/siusshotdata.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
