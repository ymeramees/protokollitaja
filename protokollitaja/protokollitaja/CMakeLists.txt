qt_add_executable(Protokollitaja WIN32 MACOSX_BUNDLE
    ../../common/src/ageclasses.cpp ../../common/src/ageclasses.h
    ../../common/src/commonsettings.cpp ../../common/src/commonsettings.h
    ../../common/src/finalsformats.cpp ../../common/src/finalsformats.h
    ../../common/src/globalvariables.h
    ../../common/src/lask.cpp ../../common/src/lask.h
    ../../common/src/qualificationevents.cpp ../../common/src/qualificationevents.h
    ../../common/src/siusdataconnection.cpp ../../common/src/siusdataconnection.h
    ../../common/src/siusdataconnections.cpp ../../common/src/siusdataconnections.h ../../common/src/siusdataconnections.ui
    ../../common/src/siusshotdata.h
    ../../common/src/targettypes.cpp ../../common/src/targettypes.h
    ../../common/src/uhendumiseaken.cpp ../../common/src/uhendumiseaken.h ../../common/src/uhendumiseaken.ui
    ../../common/src/version.h
    andmebaas.h
    competitionsettings.h
    datauploader.cpp datauploader.h
    filedownloader.cpp filedownloader.h
    finalsfileexport.cpp finalsfileexport.h finalsfileexport.ui
    importaken.cpp importaken.h importaken.ui
    initialdialog.cpp initialdialog.h initialdialog.ui
    kllfilerw.cpp kllfilerw.h
    laskudeaken.cpp laskudeaken.h laskudeaken.ui
    laskur.cpp laskur.h
    lehelugejaaken.cpp lehelugejaaken.h lehelugejaaken.ui
    leht.cpp leht.h
    liikmetevalikkast.cpp liikmetevalikkast.h liikmetevalikkast.ui
    lisalaskudeaken.cpp lisalaskudeaken.h lisalaskudeaken.ui
    main.cpp
    programname.h
    protokollitaja.cpp protokollitaja.h
    protolehelugejaconnection.cpp protolehelugejaconnection.h
    protolehelugejaserver.cpp protolehelugejaserver.h
    seadedkast.cpp seadedkast.h seadedkast.ui
    sifriseade.cpp sifriseade.h sifriseade.ui
    simplekllfilerw.cpp simplekllfilerw.h
    startlistwriter.cpp startlistwriter.h
    tabwidgetwithsettings.h
    tulemuseaken.cpp tulemuseaken.h
    updatechecker.cpp updatechecker.h
    newtabdialog.cpp newtabdialog.h newtabdialog.ui
    voistkond.cpp voistkond.h
    ../../common/xlslib/common/overnew.cpp ../../common/xlslib/common/overnew.h
    ../../common/xlslib/common/stringtok.h
    ../../common/xlslib/common/systype.h
    ../../common/xlslib/common/timespan.h
    ../../common/xlslib/common/xls_poppack.h
    ../../common/xlslib/common/xls_pshpack1.h
    ../../common/xlslib/common/xls_pshpack2.h
    ../../common/xlslib/common/xls_pshpack4.h
    ../../common/xlslib/common/xlstypes.h
    ../../common/xlslib/common/xlsys.h
    ../../common/xlslib/oledoc/binfile.cpp ../../common/xlslib/oledoc/binfile.h
    ../../common/xlslib/oledoc/oledoc.cpp ../../common/xlslib/oledoc/oledoc.h
    ../../common/xlslib/oledoc/olefs.cpp ../../common/xlslib/oledoc/olefs.h
    ../../common/xlslib/oledoc/oleprop.cpp ../../common/xlslib/oledoc/oleprop.h
    ../../common/xlslib/xlconfig.h
    ../../common/xlslib/xlslib.h
    ../../common/xlslib/xlslib/HPSF.cpp ../../common/xlslib/xlslib/HPSF.h
    ../../common/xlslib/xlslib/assert_assist.cpp
    ../../common/xlslib/xlslib/biffsection.h
    ../../common/xlslib/xlslib/blank.cpp ../../common/xlslib/xlslib/blank.h
    ../../common/xlslib/xlslib/boolean.cpp ../../common/xlslib/xlslib/boolean.h
    ../../common/xlslib/xlslib/cbridge.cpp ../../common/xlslib/xlslib/cbridge.h
    ../../common/xlslib/xlslib/cell.cpp ../../common/xlslib/xlslib/cell.h
    ../../common/xlslib/xlslib/colinfo.cpp ../../common/xlslib/xlslib/colinfo.h
    ../../common/xlslib/xlslib/colors.cpp ../../common/xlslib/xlslib/colors.h
    ../../common/xlslib/xlslib/common.h
    ../../common/xlslib/xlslib/continue.cpp ../../common/xlslib/xlslib/continue.h
    ../../common/xlslib/xlslib/datast.cpp ../../common/xlslib/xlslib/datast.h
    ../../common/xlslib/xlslib/docsumminfo.cpp ../../common/xlslib/xlslib/docsumminfo.h
    ../../common/xlslib/xlslib/err.cpp ../../common/xlslib/xlslib/err.h
    ../../common/xlslib/xlslib/extformat.cpp ../../common/xlslib/xlslib/extformat.h
    ../../common/xlslib/xlslib/font.cpp ../../common/xlslib/xlslib/font.h
    ../../common/xlslib/xlslib/format.cpp ../../common/xlslib/xlslib/format.h
    ../../common/xlslib/xlslib/formtags.h
    ../../common/xlslib/xlslib/formula.cpp ../../common/xlslib/xlslib/formula.h
    ../../common/xlslib/xlslib/formula_cell.cpp ../../common/xlslib/xlslib/formula_cell.h
    ../../common/xlslib/xlslib/formula_const.h
    ../../common/xlslib/xlslib/formula_estimate.cpp ../../common/xlslib/xlslib/formula_estimate.h
    ../../common/xlslib/xlslib/formula_expr.cpp ../../common/xlslib/xlslib/formula_expr.h
    ../../common/xlslib/xlslib/globalrec.cpp ../../common/xlslib/xlslib/globalrec.h
    ../../common/xlslib/xlslib/index.cpp ../../common/xlslib/xlslib/index.h
    ../../common/xlslib/xlslib/label.cpp ../../common/xlslib/xlslib/label.h
    ../../common/xlslib/xlslib/merged.cpp ../../common/xlslib/xlslib/merged.h
    ../../common/xlslib/xlslib/note.cpp ../../common/xlslib/xlslib/note.h
    ../../common/xlslib/xlslib/number.cpp ../../common/xlslib/xlslib/number.h
    ../../common/xlslib/xlslib/range.cpp ../../common/xlslib/xlslib/range.h
    ../../common/xlslib/xlslib/recdef.cpp ../../common/xlslib/xlslib/recdef.h
    ../../common/xlslib/xlslib/record.cpp ../../common/xlslib/xlslib/record.h
    ../../common/xlslib/xlslib/rectypes.h
    ../../common/xlslib/xlslib/row.cpp ../../common/xlslib/xlslib/row.h
    ../../common/xlslib/xlslib/sheetrec.cpp ../../common/xlslib/xlslib/sheetrec.h
    ../../common/xlslib/xlslib/summinfo.cpp ../../common/xlslib/xlslib/summinfo.h
    ../../common/xlslib/xlslib/tostr.h
    ../../common/xlslib/xlslib/unit.cpp ../../common/xlslib/xlslib/unit.h
    ../../common/xlslib/xlslib/workbook.cpp ../../common/xlslib/xlslib/workbook.h
)
target_include_directories(Protokollitaja PRIVATE
    +=
    ../../common/src
    ../../common/xlslib
    /
    INCLUDEPATH
)

target_compile_definitions(Protokollitaja PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(Protokollitaja PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::PrintSupport
    Qt::SerialPort
    Qt::Sql
)

qt_add_translations(${CMAKE_PROJECT_NAME}
    TS_FILES languages/Eesti.ts languages/English.ts languages/Hungarian.ts
    TS_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/languages"
    QM_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/languages"
    TS_FILES_OUTPUT_VARIABLE UPDATED_TS_FILES
    QM_FILES_OUTPUT_VARIABLE GENERATED_QM_FILES
)

add_dependencies(${CMAKE_PROJECT_NAME} update_translations)

# Resources:
set_source_files_properties("../../protofinaal/templates/eventFormats.json"
    PROPERTIES QT_RESOURCE_ALIAS "eventFormats.json"
)
set(protokollitaja_resource_files
    "../../protofinaal/templates/eventFormats.json"
    "../images/Abi.bmp"
    "../images/Finaal.ico"
    "../images/Protokollitaja.ico"
    "../images/copy.png"
    "../images/cut.png"
    "../images/eemaldaLaskur.png"
    "../images/eemaldaMargid.png"
    "../images/eemaldaTab.png"
    "../images/eksport.png"
    "../images/exit.png"
    "../images/finaal.png"
    "../images/import.png"
    "../images/kopeeriLaskur.png"
    "../images/margista.png"
    "../images/new.png"
    "../images/open.png"
    "../images/paste.png"
    "../images/print.png"
    "../images/reasta.png"
    "../images/salvesta_kui.png"
    "../images/save.png"
    "../images/teisaldaLaskur.png"
    "../images/tulemuse_aken.png"
    "../images/uusLaskur.png"
    "../images/uusTab.png"
    "../protokollitaja/languages/Eesti.qm"
    "../protokollitaja/languages/English.qm"
    "../protokollitaja/languages/Hungarian.qm"
)

qt_add_resources(Protokollitaja "protokollitaja"
    PREFIX
        "/"
    BASE
        ".."
    FILES
        ${protokollitaja_resource_files}
)

if(WIN32)
    target_link_libraries(Protokollitaja PRIVATE
        WrapIconv::WrapIconv
    )
endif()

install(TARGETS Protokollitaja
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET Protokollitaja
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
