qt_add_executable(Protolehelugeja WIN32 MACOSX_BUNDLE
    ../../common/src/lask.cpp ../../common/src/lask.h
    ../../common/src/target.cpp ../../common/src/target.h
    lehelugeja.cpp lehelugeja.h lehelugeja.ui
    logandcmdwindow.cpp logandcmdwindow.h logandcmdwindow.ui
    main.cpp
    scoringmachineconnection.cpp scoringmachineconnection.h
    selftests.cpp selftests.h
    version.h
)
target_include_directories(Protolehelugeja PRIVATE
    +=
    ../../common/src
    /
    INCLUDEPATH
)

target_link_libraries(Protolehelugeja PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
    Qt::SerialPort
)

qt_add_translations(${CMAKE_PROJECT_NAME}
    TS_FILES languages/Eesti.ts languages/English.ts
    TS_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/languages"
    QM_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/languages"
    TS_FILES_OUTPUT_VARIABLE UPDATED_TS_FILES
    QM_FILES_OUTPUT_VARIABLE GENERATED_QM_FILES
)

add_dependencies(${CMAKE_PROJECT_NAME} update_translations)

# Resources:
set(protolehelugeja_resource_files
    "../images/Protolehelugeja.ico"
)

qt_add_resources(Protolehelugeja "protolehelugeja"
    PREFIX
        "/"
    BASE
        ".."
    FILES
        ${protolehelugeja_resource_files}
)

install(TARGETS Protolehelugeja
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET Protolehelugeja
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
