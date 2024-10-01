qt_add_executable(siusdataconnectiontest WIN32 MACOSX_BUNDLE
    tst_siusdataconnection.cpp
    ../../src/commonsettings.cpp ../../src/commonsettings.h
    ../../src/lask.cpp ../../src/lask.h
    ../../src/siusdataconnection.cpp ../../src/siusdataconnection.h
)
target_include_directories(siusdataconnectiontest PRIVATE
    ../../src
)

target_link_libraries(siusdataconnectiontest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::Test
)

install(TARGETS siusdataconnectiontest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET siusdataconnectiontest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
