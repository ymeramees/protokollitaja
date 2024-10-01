qt_add_executable(ProtofinaalTest WIN32 MACOSX_BUNDLE
    ../../protofinaal/../../common/src/commonsettings.cpp ../../protofinaal/../../common/src/commonsettings.h
    ../../protofinaal/../../common/src/finalsformats.cpp ../../protofinaal/../../common/src/finalsformats.h
    ../../protofinaal/../../common/src/lask.cpp ../../protofinaal/../../common/src/lask.h
    ../../protofinaal/../../common/src/siusdataconnection.cpp ../../protofinaal/../../common/src/siusdataconnection.h
    ../../protofinaal/../../common/src/siusdataconnections.cpp ../../protofinaal/../../common/src/siusdataconnections.h ../../protofinaal/../../common/src/siusdataconnections.ui
    ../../protofinaal/competitor.cpp ../../protofinaal/competitor.h
    ../../protofinaal/initialdialog.cpp ../../protofinaal/initialdialog.h ../../protofinaal/initialdialog.ui
    ../../protofinaal/protofinaal.cpp ../../protofinaal/protofinaal.h
    ../../protofinaal/shotedit.cpp ../../protofinaal/shotedit.h
    ../../protofinaal/spectatorwindow.cpp ../../protofinaal/spectatorwindow.h
    ../../protofinaal/team.cpp ../../protofinaal/team.h
    ../../protofinaal/teamstable.cpp ../../protofinaal/teamstable.h
    tst_protofinaaltest.cpp
)
target_include_directories(ProtofinaalTest PRIVATE
    +=
    ../../../common/src
    ../../protofinaal ../../protofinaal
    INCLUDEPATH
)

target_link_libraries(ProtofinaalTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::Test
)


# Resources:
set_source_files_properties("../../protofinaal/../templates/eventFormats.json"
    PROPERTIES QT_RESOURCE_ALIAS "eventFormats.json"
)
set_source_files_properties("../../protofinaal/../templates/spectatorView_template.html"
    PROPERTIES QT_RESOURCE_ALIAS "spectatorView_template.html"
)
set(protofinaal_resource_files
    "../../protofinaal/../templates/eventFormats.json"
    "../../protofinaal/../templates/spectatorView_template.html"
)

qt_add_resources(ProtofinaalTest "protofinaal"
    PREFIX
        "/"
    BASE
        "../../protofinaal/.."
    FILES
        ${protofinaal_resource_files}
)
set_source_files_properties("../resources/testFromProtokollitaja.fin"
    PROPERTIES QT_RESOURCE_ALIAS "testFromProtokollitaja.fin"
)
set_source_files_properties("../resources/testWithPoints.fin"
    PROPERTIES QT_RESOURCE_ALIAS "testWithPoints.fin"
)
set_source_files_properties("../resources/testWithShots.fin"
    PROPERTIES QT_RESOURCE_ALIAS "testWithShots.fin"
)
set(testResources_resource_files
    "../resources/testFromProtokollitaja.fin"
    "../resources/testWithPoints.fin"
    "../resources/testWithShots.fin"
)

qt_add_resources(ProtofinaalTest "testResources"
    PREFIX
        "/"
    BASE
        ".."
    FILES
        ${testResources_resource_files}
)

install(TARGETS ProtofinaalTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ProtofinaalTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
