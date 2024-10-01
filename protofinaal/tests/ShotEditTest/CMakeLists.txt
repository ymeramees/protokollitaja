qt_add_executable(ShotEditTest WIN32 MACOSX_BUNDLE
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../protofinaal/shotedit.cpp ../../protofinaal/shotedit.h
    tst_shotedittest.cpp
)
target_include_directories(ShotEditTest PRIVATE
    ../../../common/src
    ../../protofinaal
)

target_link_libraries(ShotEditTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Test
)

install(TARGETS ShotEditTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ShotEditTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
