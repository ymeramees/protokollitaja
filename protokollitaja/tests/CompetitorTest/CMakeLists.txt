qt_add_executable(CompetitorTest
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../protokollitaja/laskudeaken.cpp ../../protokollitaja/laskudeaken.h ../../protokollitaja/laskudeaken.ui
    ../../protokollitaja/laskur.cpp ../../protokollitaja/laskur.h
    ../../protokollitaja/lisalaskudeaken.cpp ../../protokollitaja/lisalaskudeaken.h ../../protokollitaja/lisalaskudeaken.ui
    tst_competitor.cpp
)
target_include_directories(CompetitorTest PRIVATE
    ../../../common/src
    ../../protokollitaja
)

target_link_libraries(CompetitorTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)

install(TARGETS CompetitorTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET CompetitorTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
