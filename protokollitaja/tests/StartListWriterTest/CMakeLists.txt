qt_add_executable(StartListWriterTest WIN32 MACOSX_BUNDLE
    ../../../common/src/commonsettings.cpp ../../../common/src/commonsettings.h
    ../../protokollitaja/startlistwriter.cpp ../../protokollitaja/startlistwriter.h
    tst_startlistwritertest.cpp
)
target_include_directories(StartListWriterTest PRIVATE
    ../../../common/src
    ../../protokollitaja
)

target_link_libraries(StartListWriterTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Test
)

install(TARGETS StartListWriterTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET StartListWriterTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
