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
)
target_include_directories(Protofinaal PRIVATE
    +=
    ../../common/src
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
