qt_add_executable(TargetTest
    ../../src/lask.cpp ../../src/lask.h
    ../../src/target.cpp ../../src/target.h
    ../../src/targettypes.cpp ../../src/targettypes.h
    tst_target.cpp
)
target_include_directories(TargetTest PRIVATE
    ../../src
)

target_link_libraries(TargetTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)

install(TARGETS TargetTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TargetTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
