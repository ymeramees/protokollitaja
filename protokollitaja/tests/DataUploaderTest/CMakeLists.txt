qt_add_executable(DataUploaderTest
    ../../protokollitaja/datauploader.cpp ../../protokollitaja/datauploader.h
    tst_datauploadertest.cpp
)
target_include_directories(DataUploaderTest PRIVATE
    ../../protokollitaja
)

target_link_libraries(DataUploaderTest PRIVATE
    Qt::Core
    Qt::Network
    Qt::Test
)

install(TARGETS DataUploaderTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET DataUploaderTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
