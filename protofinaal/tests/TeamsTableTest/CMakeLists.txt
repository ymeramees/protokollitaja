qt_add_executable(TeamsTableTest WIN32 MACOSX_BUNDLE
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../protofinaal/competitor.cpp ../../protofinaal/competitor.h
    ../../protofinaal/shotedit.cpp ../../protofinaal/shotedit.h
    ../../protofinaal/team.cpp ../../protofinaal/team.h
    ../../protofinaal/teamstable.cpp ../../protofinaal/teamstable.h
    tst_teamstabletest.cpp
)
target_include_directories(TeamsTableTest PRIVATE
    +=
    ../../../common/src
    ../../protofinaal
    INCLUDEPATH
)

target_link_libraries(TeamsTableTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Test
)

install(TARGETS TeamsTableTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET TeamsTableTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
