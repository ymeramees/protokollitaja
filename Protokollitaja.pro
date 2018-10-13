TEMPLATE = app
TARGET = Protokollitaja
INCLUDEPATH += ./src \
	/src/xlslib \
DEPENDPATH += $${INCLUDEPATH}

include(qextserialport/src/qextserialport.pri)

QT += core \
    gui \
    sql \
    network \
    printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += lisalaskudeaken.h \
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
    xlslib.h \
    xlconfig.h \
    common/xlsys.h \
    common/xlstypes.h \
    common/xls_pshpack4.h \
    common/xls_pshpack2.h \
    common/xls_pshpack1.h \
    common/xls_poppack.h \
    common/timespan.h \
    common/systype.h \
    common/stringtok.h \
    common/overnew.h \
    oledoc/oleprop.h \
    oledoc/olefs.h \
    oledoc/oledoc.h \
    oledoc/binfile.h \
    xlslib/workbook.h \
    xlslib/unit.h \
    xlslib/tostr.h \
    xlslib/summinfo.h \
    xlslib/sheetrec.h \
    xlslib/row.h \
    xlslib/rectypes.h \
    xlslib/record.h \
    xlslib/recdef.h \
    xlslib/range.h \
    xlslib/number.h \
    xlslib/note.h \
    xlslib/merged.h \
    xlslib/label.h \
    xlslib/index.h \
    xlslib/HPSF.h \
    xlslib/globalrec.h \
    xlslib/formula_expr.h \
    xlslib/formula_estimate.h \
    xlslib/formula_const.h \
    xlslib/formula_cell.h \
    xlslib/formula.h \
    xlslib/formtags.h \
    xlslib/format.h \
    xlslib/font.h \
    xlslib/extformat.h \
    xlslib/err.h \
    xlslib/docsumminfo.h \
    xlslib/datast.h \
    xlslib/continue.h \
    xlslib/common.h \
    xlslib/colors.h \
    xlslib/colinfo.h \
    xlslib/cell.h \
    xlslib/cbridge.h \
    xlslib/boolean.h \
    xlslib/blank.h \
    xlslib/biffsection.h \
    lask.h \
    version.h \
    laskudeaken.h \
    ../QFinaal/uhendumiseaken.h \
    filedownloader.h
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
    common/overnew.cpp \
    oledoc/oleprop.cpp \
    oledoc/olefs.cpp \
    oledoc/oledoc.cpp \
    oledoc/binfile.cpp \
    xlslib/workbook.cpp \
    xlslib/unit.cpp \
    xlslib/summinfo.cpp \
    xlslib/sheetrec.cpp \
    xlslib/row.cpp \
    xlslib/record.cpp \
    xlslib/recdef.cpp \
    xlslib/range.cpp \
    xlslib/number.cpp \
    xlslib/note.cpp \
    xlslib/merged.cpp \
    xlslib/label.cpp \
    xlslib/index.cpp \
    xlslib/HPSF.cpp \
    xlslib/globalrec.cpp \
    xlslib/formula_expr.cpp \
    xlslib/formula_estimate.cpp \
    xlslib/formula_cell.cpp \
    xlslib/formula.cpp \
    xlslib/format.cpp \
    xlslib/font.cpp \
    xlslib/extformat.cpp \
    xlslib/err.cpp \
    xlslib/docsumminfo.cpp \
    xlslib/datast.cpp \
    xlslib/continue.cpp \
    xlslib/colors.cpp \
    xlslib/colinfo.cpp \
    xlslib/cell.cpp \
    xlslib/cbridge.cpp \
    xlslib/boolean.cpp \
    xlslib/blank.cpp \
    xlslib/assert_assist.cpp \
    lask.cpp \
    laskudeaken.cpp \
    ../QFinaal/uhendumiseaken.cpp \
    filedownloader.cpp
FORMS += lisalaskudeaken.ui \
    liikmetevalikkast.ui \
    importaken.ui \
    algusevalik.ui \
    seadedkast.ui \
    valikkast.ui \
    lehelugejaaken.ui \
    sifriseade.ui \
    laskudeaken.ui \
    ../QFinaal/uhendumiseaken.ui
RESOURCES += protokollitaja.qrc
RC_FILE += ikoon.rc
win32{
LIBS += -liconv #-L../libxl-2.2.0/bin -lxl \
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
