qt_add_executable(RangeControl WIN32 MACOSX_BUNDLE
    ../../common/src/lask.cpp ../../common/src/lask.h
    ../../common/src/qualificationevents.cpp ../../common/src/qualificationevents.h
    ../../common/src/siusshotdata.h
    ../../common/src/utils.cpp ../../common/src/utils.h
    ../../common/src/version.h
    connectionserver.cpp connectionserver.h
    dataconnection.cpp dataconnection.h
    header.cpp header.h
    inbandconnection.cpp inbandconnection.h
    lane.cpp lane.h
    main.cpp
    programname.h
    rangecontrol.cpp rangecontrol.h
)
target_include_directories(RangeControl PRIVATE
    +=
    ../../common/src
    /
    INCLUDEPATH
)

target_link_libraries(RangeControl PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
)


# Resources:
set(resources_resource_files
    "languages/Eesti.qm"
    "languages/English.qm"
)

qt_add_resources(RangeControl "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)

install(TARGETS RangeControl
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET RangeControl
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
