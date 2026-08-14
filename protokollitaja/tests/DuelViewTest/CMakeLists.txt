qt_add_executable(DuelViewTest
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../../common/src/target.cpp ../../../common/src/target.h
    ../../../common/src/targettypes.cpp ../../../common/src/targettypes.h
    ../../protokollitaja/duelcompetitorview.cpp ../../protokollitaja/duelcompetitorview.h
    ../../protokollitaja/duelpair.cpp ../../protokollitaja/duelpair.h
    ../../protokollitaja/duelview.cpp ../../protokollitaja/duelview.h
    ../../protokollitaja/laskudeaken.cpp ../../protokollitaja/laskudeaken.h ../../protokollitaja/laskudeaken.ui
    ../../protokollitaja/laskur.cpp ../../protokollitaja/laskur.h
    ../../protokollitaja/leht.cpp ../../protokollitaja/leht.h
    ../../protokollitaja/liikmetevalikkast.cpp ../../protokollitaja/liikmetevalikkast.h ../../protokollitaja/liikmetevalikkast.ui
    ../../protokollitaja/lisalaskudeaken.cpp ../../protokollitaja/lisalaskudeaken.h ../../protokollitaja/lisalaskudeaken.ui
    ../../protokollitaja/voistkond.cpp ../../protokollitaja/voistkond.h
    tst_duelview.cpp
)
target_include_directories(DuelViewTest PRIVATE
    ../../../common/src
    ../../protokollitaja
)

target_link_libraries(DuelViewTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)

install(TARGETS DuelViewTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET DuelViewTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
