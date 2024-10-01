qt_add_executable(UpdateCheckerTest
    ../../protokollitaja/filedownloader.cpp ../../protokollitaja/filedownloader.h
    ../../protokollitaja/updatechecker.cpp ../../protokollitaja/updatechecker.h
    tst_updatechecker.cpp
)
target_include_directories(UpdateCheckerTest PRIVATE
    ../../protokollitaja
)

target_link_libraries(UpdateCheckerTest PRIVATE
    Qt::Core
    Qt::Network
    Qt::Test
)

install(TARGETS UpdateCheckerTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET UpdateCheckerTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
