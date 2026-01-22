qt_add_executable(FinalsFileExportTest
    ../../../common/src/commonsettings.cpp ../../../common/src/commonsettings.h
    ../../../common/src/finalsformats.cpp ../../../common/src/finalsformats.h
    ../../protokollitaja/finalsfileexport.cpp ../../protokollitaja/finalsfileexport.h ../../protokollitaja/finalsfileexport.ui
    ../../protokollitaja/startlistwriter.cpp ../../protokollitaja/startlistwriter.h
    ../../../common/src/qualificationevents.cpp ../../../common/src/qualificationevents.h
    tst_finalsfileexport.cpp
)
target_include_directories(FinalsFileExportTest PRIVATE
    ../../../common/src
    ../../protokollitaja
)

target_link_libraries(FinalsFileExportTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)


# Resources:
set_source_files_properties("../../../protofinaal/templates/eventFormats.json"
    PROPERTIES QT_RESOURCE_ALIAS "eventFormats.json"
)
set(protokollitaja_resource_files
    "../../../protofinaal/templates/eventFormats.json"
    "../../images/Abi.bmp"
    "../../images/Finaal.ico"
    "../../images/Protokollitaja.ico"
    "../../images/copy.png"
    "../../images/cut.png"
    "../../images/eemaldaLaskur.png"
    "../../images/eemaldaMargid.png"
    "../../images/eemaldaTab.png"
    "../../images/eksport.png"
    "../../images/exit.png"
    "../../images/finaal.png"
    "../../images/import.png"
    "../../images/kopeeriLaskur.png"
    "../../images/margista.png"
    "../../images/new.png"
    "../../images/open.png"
    "../../images/paste.png"
    "../../images/print.png"
    "../../images/reasta.png"
    "../../images/salvesta_kui.png"
    "../../images/save.png"
    "../../images/teisaldaLaskur.png"
    "../../images/tulemuse_aken.png"
    "../../images/uusLaskur.png"
    "../../images/uusTab.png"
    "../../protokollitaja/languages/Eesti.qm"
    "../../protokollitaja/languages/English.qm"
    "../../protokollitaja/languages/Hungarian.qm"
)

qt_add_resources(FinalsFileExportTest "protokollitaja"
    PREFIX
        "/"
    BASE
        "../.."
    FILES
        ${protokollitaja_resource_files}
)

install(TARGETS FinalsFileExportTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET FinalsFileExportTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
