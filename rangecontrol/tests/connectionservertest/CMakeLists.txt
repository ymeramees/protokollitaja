qt_add_executable(connectionservertest
    ../../rangecontrol/../../common/src/lask.cpp ../../rangecontrol/../../common/src/lask.h
    ../../rangecontrol/../../common/src/qualificationevents.cpp ../../rangecontrol/../../common/src/qualificationevents.h
    ../../rangecontrol/../../common/src/siusshotdata.h
    ../../rangecontrol/../../common/src/utils.cpp ../../rangecontrol/../../common/src/utils.h
    ../../rangecontrol/connectionserver.cpp ../../rangecontrol/connectionserver.h
    ../../rangecontrol/dataconnection.cpp ../../rangecontrol/dataconnection.h
    ../../rangecontrol/inbandconnection.cpp ../../rangecontrol/inbandconnection.h
    tst_connectionservertest.cpp
)
target_include_directories(connectionservertest PRIVATE
    ../../../common/src
    ../../rangecontrol ../../rangecontrol
)

target_link_libraries(connectionservertest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::Test
)

install(TARGETS connectionservertest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET connectionservertest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
