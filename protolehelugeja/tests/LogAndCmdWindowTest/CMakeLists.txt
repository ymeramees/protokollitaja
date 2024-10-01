qt_add_executable(LogAndCmdWindowTest
    ../../protolehelugeja/logandcmdwindow.cpp ../../protolehelugeja/logandcmdwindow.h ../../protolehelugeja/logandcmdwindow.ui
    tst_logandcmdwindow.cpp
)
target_include_directories(LogAndCmdWindowTest PRIVATE
    ../../protolehelugeja
)

target_link_libraries(LogAndCmdWindowTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)

install(TARGETS LogAndCmdWindowTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET LogAndCmdWindowTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
