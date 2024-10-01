qt_add_executable(scoringmachineconnectiontest WIN32 MACOSX_BUNDLE
    tst_scoringmachineconnection.cpp
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../protolehelugeja/scoringmachineconnection.cpp ../../protolehelugeja/scoringmachineconnection.h
)
target_include_directories(scoringmachineconnectiontest PRIVATE
    ../../../common/src
    ../../protolehelugeja
)

target_link_libraries(scoringmachineconnectiontest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::SerialPort
    Qt::Test
)

install(TARGETS scoringmachineconnectiontest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET scoringmachineconnectiontest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
