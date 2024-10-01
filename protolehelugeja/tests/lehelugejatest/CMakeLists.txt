qt_add_executable(lehelugejatest WIN32 MACOSX_BUNDLE
    tst_lehelugeja.cpp
    ../../protolehelugeja/../../common/src/lask.cpp ../../protolehelugeja/../../common/src/lask.h
    ../../protolehelugeja/../../common/src/target.cpp ../../protolehelugeja/../../common/src/target.h
    ../../protolehelugeja/lehelugeja.cpp ../../protolehelugeja/lehelugeja.h ../../protolehelugeja/lehelugeja.ui
    ../../protolehelugeja/logandcmdwindow.cpp ../../protolehelugeja/logandcmdwindow.h ../../protolehelugeja/logandcmdwindow.ui
    ../../protolehelugeja/scoringmachineconnection.cpp ../../protolehelugeja/scoringmachineconnection.h
    ../../protolehelugeja/selftests.cpp ../../protolehelugeja/selftests.h
    ../../protolehelugeja/version.h
)
target_include_directories(lehelugejatest PRIVATE
    ../../protolehelugeja ../../protolehelugeja
    ../../../common/src
)

target_link_libraries(lehelugejatest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::SerialPort
    Qt::Test
)


# Resources:
set(protolehelugeja_resource_files
    "../../protolehelugeja/../images/Protolehelugeja.ico"
)

qt_add_resources(lehelugejatest "protolehelugeja"
    PREFIX
        "/"
    BASE
        "../../protolehelugeja/.."
    FILES
        ${protolehelugeja_resource_files}
)

install(TARGETS lehelugejatest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET lehelugejatest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
