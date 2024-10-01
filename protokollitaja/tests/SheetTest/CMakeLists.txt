qt_add_executable(SheetTest
    ../../../common/src/lask.cpp ../../../common/src/lask.h
    ../../protokollitaja/laskudeaken.cpp ../../protokollitaja/laskudeaken.h ../../protokollitaja/laskudeaken.ui
    ../../protokollitaja/laskur.cpp ../../protokollitaja/laskur.h
    ../../protokollitaja/leht.cpp ../../protokollitaja/leht.h
    ../../protokollitaja/liikmetevalikkast.cpp ../../protokollitaja/liikmetevalikkast.h ../../protokollitaja/liikmetevalikkast.ui
    ../../protokollitaja/lisalaskudeaken.cpp ../../protokollitaja/lisalaskudeaken.h ../../protokollitaja/lisalaskudeaken.ui
    ../../protokollitaja/voistkond.cpp ../../protokollitaja/voistkond.h
    tst_sheet.cpp
)
target_include_directories(SheetTest PRIVATE
    ../../../common/src
    ../../protokollitaja
)

target_link_libraries(SheetTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)

install(TARGETS SheetTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET SheetTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
