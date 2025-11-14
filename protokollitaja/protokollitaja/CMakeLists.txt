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
    valikkast.cpp valikkast.h valikkast.ui
    voistkond.cpp voistkond.h
    xlslib/common/overnew.cpp xlslib/common/overnew.h
    xlslib/common/stringtok.h
    xlslib/common/systype.h
    xlslib/common/timespan.h
    xlslib/common/xls_poppack.h
    xlslib/common/xls_pshpack1.h
    xlslib/common/xls_pshpack2.h
    xlslib/common/xls_pshpack4.h
    xlslib/common/xlstypes.h
    xlslib/common/xlsys.h
    xlslib/oledoc/binfile.cpp xlslib/oledoc/binfile.h
    xlslib/oledoc/oledoc.cpp xlslib/oledoc/oledoc.h
    xlslib/oledoc/olefs.cpp xlslib/oledoc/olefs.h
    xlslib/oledoc/oleprop.cpp xlslib/oledoc/oleprop.h
    xlslib/xlconfig.h
    xlslib/xlslib.h
    xlslib/xlslib/HPSF.cpp xlslib/xlslib/HPSF.h
    xlslib/xlslib/assert_assist.cpp
    xlslib/xlslib/biffsection.h
    xlslib/xlslib/blank.cpp xlslib/xlslib/blank.h
    xlslib/xlslib/boolean.cpp xlslib/xlslib/boolean.h
    xlslib/xlslib/cbridge.cpp xlslib/xlslib/cbridge.h
    xlslib/xlslib/cell.cpp xlslib/xlslib/cell.h
    xlslib/xlslib/colinfo.cpp xlslib/xlslib/colinfo.h
    xlslib/xlslib/colors.cpp xlslib/xlslib/colors.h
    xlslib/xlslib/common.h
    xlslib/xlslib/continue.cpp xlslib/xlslib/continue.h
    xlslib/xlslib/datast.cpp xlslib/xlslib/datast.h
    xlslib/xlslib/docsumminfo.cpp xlslib/xlslib/docsumminfo.h
    xlslib/xlslib/err.cpp xlslib/xlslib/err.h
    xlslib/xlslib/extformat.cpp xlslib/xlslib/extformat.h
    xlslib/xlslib/font.cpp xlslib/xlslib/font.h
    xlslib/xlslib/format.cpp xlslib/xlslib/format.h
    xlslib/xlslib/formtags.h
    xlslib/xlslib/formula.cpp xlslib/xlslib/formula.h
    xlslib/xlslib/formula_cell.cpp xlslib/xlslib/formula_cell.h
    xlslib/xlslib/formula_const.h
    xlslib/xlslib/formula_estimate.cpp xlslib/xlslib/formula_estimate.h
    xlslib/xlslib/formula_expr.cpp xlslib/xlslib/formula_expr.h
    xlslib/xlslib/globalrec.cpp xlslib/xlslib/globalrec.h
    xlslib/xlslib/index.cpp xlslib/xlslib/index.h
    xlslib/xlslib/label.cpp xlslib/xlslib/label.h
    xlslib/xlslib/merged.cpp xlslib/xlslib/merged.h
    xlslib/xlslib/note.cpp xlslib/xlslib/note.h
    xlslib/xlslib/number.cpp xlslib/xlslib/number.h
    xlslib/xlslib/range.cpp xlslib/xlslib/range.h
    xlslib/xlslib/recdef.cpp xlslib/xlslib/recdef.h
    xlslib/xlslib/record.cpp xlslib/xlslib/record.h
    xlslib/xlslib/rectypes.h
    xlslib/xlslib/row.cpp xlslib/xlslib/row.h
    xlslib/xlslib/sheetrec.cpp xlslib/xlslib/sheetrec.h
    xlslib/xlslib/summinfo.cpp xlslib/xlslib/summinfo.h
    xlslib/xlslib/tostr.h
    xlslib/xlslib/unit.cpp xlslib/xlslib/unit.h
    xlslib/xlslib/workbook.cpp xlslib/xlslib/workbook.h
)
target_include_directories(Protokollitaja PRIVATE
    +=
    ../../common/src
    /
    INCLUDEPATH
    xlslib
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
    TS_FILES languages/Eesti.ts languages/English.ts
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
