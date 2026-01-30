qt_add_executable(Protofinaal WIN32 MACOSX_BUNDLE
    ../../common/src/commonsettings.cpp ../../common/src/commonsettings.h
    ../../common/src/finalsformats.cpp ../../common/src/finalsformats.h
    ../../common/src/lask.cpp ../../common/src/lask.h
    ../../common/src/siusdataconnection.cpp ../../common/src/siusdataconnection.h
    ../../common/src/siusdataconnections.cpp ../../common/src/siusdataconnections.h ../../common/src/siusdataconnections.ui
    competitor.cpp competitor.h
    initialdialog.cpp initialdialog.h initialdialog.ui
    main.cpp
    protofinaal.cpp protofinaal.h
    shotedit.cpp shotedit.h
    spectatorwindow.cpp spectatorwindow.h
    team.cpp team.h
    teamstable.cpp teamstable.h
    xlsexportservice.cpp xlsexportservice.h
    xlsexportdata.cpp xlsexportdata.h
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
target_include_directories(Protofinaal PRIVATE
    +=
    ../../common/src
    ../../common/xlslib
    /
    INCLUDEPATH
)

target_compile_definitions(Protofinaal PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(Protofinaal PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
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
set_source_files_properties("../templates/eventFormats.json"
    PROPERTIES QT_RESOURCE_ALIAS "eventFormats.json"
)
set_source_files_properties("../templates/spectatorView_template.html"
    PROPERTIES QT_RESOURCE_ALIAS "spectatorView_template.html"
)
set(protofinaal_resource_files
    "../templates/eventFormats.json"
    "../templates/spectatorView_template.html"
)

qt_add_resources(Protofinaal "protofinaal"
    PREFIX
        "/"
    BASE
        ".."
    FILES
        ${protofinaal_resource_files}
)
set(resources_resource_files
    "languages/Eesti.qm"
    "languages/English.qm"
    "languages/Hungarian.qm"
)

qt_add_resources(Protofinaal "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)

install(TARGETS Protofinaal
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET Protofinaal
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
