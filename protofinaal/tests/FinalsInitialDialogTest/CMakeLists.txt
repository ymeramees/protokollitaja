qt_add_executable(FinalsInitialDialogTest WIN32 MACOSX_BUNDLE
    ../../../common/src/finalsformats.cpp ../../../common/src/finalsformats.h
    ../../protofinaal/initialdialog.cpp ../../protofinaal/initialdialog.h ../../protofinaal/initialdialog.ui
    tst_initialdialogtest.cpp
)
target_include_directories(FinalsInitialDialogTest PRIVATE
    ../../../common/src
    ../../protofinaal
)

target_link_libraries(FinalsInitialDialogTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Test
)


# Resources:
set_source_files_properties("../../templates/eventFormats.json"
    PROPERTIES QT_RESOURCE_ALIAS "eventFormats.json"
)
set_source_files_properties("../../templates/spectatorView_template.html"
    PROPERTIES QT_RESOURCE_ALIAS "spectatorView_template.html"
)
set(protofinaal_resource_files
    "../../templates/eventFormats.json"
    "../../templates/spectatorView_template.html"
)

qt_add_resources(FinalsInitialDialogTest "protofinaal"
    PREFIX
        "/"
    BASE
        "../.."
    FILES
        ${protofinaal_resource_files}
)

install(TARGETS FinalsInitialDialogTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET FinalsInitialDialogTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
