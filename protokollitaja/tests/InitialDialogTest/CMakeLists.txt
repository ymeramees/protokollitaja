qt_add_executable(InitialDialogTest WIN32 MACOSX_BUNDLE
    ../../protokollitaja/initialdialog.cpp ../../protokollitaja/initialdialog.h ../../protokollitaja/initialdialog.ui
    ../../protokollitaja/simplekllfilerw.cpp ../../protokollitaja/simplekllfilerw.h
    tst_initialdialogtest.cpp
)
target_include_directories(InitialDialogTest PRIVATE
    ../../protokollitaja
)

target_link_libraries(InitialDialogTest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Test
)

install(TARGETS InitialDialogTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET InitialDialogTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
