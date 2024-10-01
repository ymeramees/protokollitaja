qt_add_executable(CommonSettingsTest
    ../../src/commonsettings.cpp ../../src/commonsettings.h
    tst_commonsettings.cpp
)
target_include_directories(CommonSettingsTest PRIVATE
    ../../src
)

target_link_libraries(CommonSettingsTest PRIVATE
    Qt::Core
    Qt::Test
)

install(TARGETS CommonSettingsTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET CommonSettingsTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
