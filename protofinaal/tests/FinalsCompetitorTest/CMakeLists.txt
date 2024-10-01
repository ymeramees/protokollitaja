qt_add_executable(FinalsCompetitorTest WIN32 MACOSX_BUNDLE
    ../../../common/src/targettypes.cpp ../../../common/src/targettypes.h
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../protofinaal/competitor.cpp ../../protofinaal/competitor.h
    ../../protofinaal/shotedit.cpp ../../protofinaal/shotedit.h
    tst_competitortest.cpp
)
target_include_directories(FinalsCompetitorTest PRIVATE
    +=
    ../../../common/src
    ../../protofinaal
    INCLUDEPATH
)

target_link_libraries(FinalsCompetitorTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Test
    Qt::Widgets
)

install(TARGETS FinalsCompetitorTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET FinalsCompetitorTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
