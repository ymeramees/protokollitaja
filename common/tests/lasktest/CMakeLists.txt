qt_add_executable(lasktest
    ../../src/lask.cpp ../../src/lask.h
    tst_lask.cpp
)
target_include_directories(lasktest PRIVATE
    ../../src
)

target_link_libraries(lasktest PRIVATE
    Qt::Core
    Qt::Test
)

install(TARGETS lasktest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET lasktest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
