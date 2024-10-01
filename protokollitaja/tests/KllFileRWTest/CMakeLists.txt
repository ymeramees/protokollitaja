qt_add_executable(KllFileRWTest
    ../../protokollitaja/competitionsettings.h
    ../../protokollitaja/simplekllfilerw.cpp ../../protokollitaja/simplekllfilerw.h
    tst_kllfilerwtest.cpp
)
target_include_directories(KllFileRWTest PRIVATE
    ../../protokollitaja
)

target_link_libraries(KllFileRWTest PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Test
)

install(TARGETS KllFileRWTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET KllFileRWTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
