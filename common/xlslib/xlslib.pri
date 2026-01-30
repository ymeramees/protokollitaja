INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

PUBLIC_HEADERS         += $$PWD/xlslib.h \
						  $$PWD/xlconfig.h

HEADERS                += $$PUBLIC_HEADERS \
                          $$PWD/common/xlsys.h \
                          $$PWD/common/xlstypes.h \
                          $$PWD/common/xls_pshpack4.h \
                          $$PWD/common/xls_pshpack2.h \
                          $$PWD/common/xls_pshpack1.h \
                          $$PWD/common/xls_poppack.h \
                          $$PWD/common/timespan.h \
                          $$PWD/common/systype.h \
                          $$PWD/common/stringtok.h \
                          $$PWD/common/overnew.h \
                          $$PWD/oledoc/oleprop.h \
                          $$PWD/oledoc/olefs.h \
                          $$PWD/oledoc/oledoc.h \
                          $$PWD/oledoc/binfile.h \
                          $$PWD/xlslib/workbook.h \
                          $$PWD/xlslib/unit.h \
                          $$PWD/xlslib/tostr.h \
                          $$PWD/xlslib/summinfo.h \
                          $$PWD/xlslib/sheetrec.h \
                          $$PWD/xlslib/row.h \
                          $$PWD/xlslib/rectypes.h \
                          $$PWD/xlslib/record.h \
                          $$PWD/xlslib/recdef.h \
                          $$PWD/xlslib/range.h \
                          $$PWD/xlslib/number.h \
                          $$PWD/xlslib/note.h \
                          $$PWD/xlslib/merged.h \
                          $$PWD/xlslib/label.h \
                          $$PWD/xlslib/index.h \
                          $$PWD/xlslib/HPSF.h \
                          $$PWD/xlslib/globalrec.h \
                          $$PWD/xlslib/formula_expr.h \
                          $$PWD/xlslib/formula_estimate.h \
                          $$PWD/xlslib/formula_const.h \
                          $$PWD/xlslib/formula_cell.h \
                          $$PWD/xlslib/formula.h \
                          $$PWD/xlslib/formtags.h \
                          $$PWD/xlslib/format.h \
                          $$PWD/xlslib/font.h \
                          $$PWD/xlslib/extformat.h \
                          $$PWD/xlslib/err.h \
                          $$PWD/xlslib/docsumminfo.h \
                          $$PWD/xlslib/datast.h \
                          $$PWD/xlslib/continue.h \
                          $$PWD/xlslib/common.h \
                          $$PWD/xlslib/colors.h \
                          $$PWD/xlslib/colinfo.h \
                          $$PWD/xlslib/cell.h \
                          $$PWD/xlslib/cbridge.h \
                          $$PWD/xlslib/boolean.h \
                          $$PWD/xlslib/blank.h \
                          $$PWD/xlslib/biffsection.h \

SOURCES                += $$PWD/common/overnew.cpp \
                          $$PWD/oledoc/oleprop.cpp \
                          $$PWD/oledoc/olefs.cpp \
                          $$PWD/oledoc/oledoc.cpp \
                          $$PWD/oledoc/binfile.cpp \
                          $$PWD/xlslib/workbook.cpp \
                          $$PWD/xlslib/unit.cpp \
                          $$PWD/xlslib/summinfo.cpp \
                          $$PWD/xlslib/sheetrec.cpp \
                          $$PWD/xlslib/row.cpp \
                          $$PWD/xlslib/record.cpp \
                          $$PWD/xlslib/recdef.cpp \
                          $$PWD/xlslib/range.cpp \
                          $$PWD/xlslib/number.cpp \
                          $$PWD/xlslib/note.cpp \
                          $$PWD/xlslib/merged.cpp \
                          $$PWD/xlslib/label.cpp \
                          $$PWD/xlslib/index.cpp \
                          $$PWD/xlslib/HPSF.cpp \
                          $$PWD/xlslib/globalrec.cpp \
                          $$PWD/xlslib/formula_expr.cpp \
                          $$PWD/xlslib/formula_estimate.cpp \
                          $$PWD/xlslib/formula_cell.cpp \
                          $$PWD/xlslib/formula.cpp \
                          $$PWD/xlslib/format.cpp \
                          $$PWD/xlslib/font.cpp \
                          $$PWD/xlslib/extformat.cpp \
                          $$PWD/xlslib/err.cpp \
                          $$PWD/xlslib/docsumminfo.cpp \
                          $$PWD/xlslib/datast.cpp \
                          $$PWD/xlslib/continue.cpp \
                          $$PWD/xlslib/colors.cpp \
                          $$PWD/xlslib/colinfo.cpp \
                          $$PWD/xlslib/cell.cpp \
                          $$PWD/xlslib/cbridge.cpp \
                          $$PWD/xlslib/boolean.cpp \
                          $$PWD/xlslib/blank.cpp \
                          $$PWD/xlslib/assert_assist.cpp \
