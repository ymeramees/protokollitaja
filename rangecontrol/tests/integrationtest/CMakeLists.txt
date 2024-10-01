qt_add_executable(integrationtest
    ../../rangecontrol/../../common/src/lask.cpp ../../rangecontrol/../../common/src/lask.h
    ../../rangecontrol/../../common/src/qualificationevents.cpp ../../rangecontrol/../../common/src/qualificationevents.h
    ../../rangecontrol/../../common/src/siusshotdata.h
    ../../rangecontrol/../../common/src/utils.cpp ../../rangecontrol/../../common/src/utils.h
    ../../rangecontrol/../../common/src/version.h
    ../../rangecontrol/connectionserver.cpp ../../rangecontrol/connectionserver.h
    ../../rangecontrol/dataconnection.cpp ../../rangecontrol/dataconnection.h
    ../../rangecontrol/header.cpp ../../rangecontrol/header.h
    ../../rangecontrol/inbandconnection.cpp ../../rangecontrol/inbandconnection.h
    ../../rangecontrol/lane.cpp ../../rangecontrol/lane.h
    ../../rangecontrol/programname.h
    ../../rangecontrol/rangecontrol.cpp ../../rangecontrol/rangecontrol.h
    tst_integrationtest.cpp
)
target_include_directories(integrationtest PRIVATE
    ../../../common/src
    ../../rangecontrol ../../rangecontrol
)

target_link_libraries(integrationtest PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::Test
)

install(TARGETS integrationtest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET integrationtest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
