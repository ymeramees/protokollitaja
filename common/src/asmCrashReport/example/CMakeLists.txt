cmake_minimum_required(VERSION 3.16)
project(asmCrashReportExample VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_executable(asmCrashReportExample WIN32 MACOSX_BUNDLE
    main.cpp
)
target_link_libraries(asmCrashReportExample PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

if(APPLE)
    set_target_properties(asmCrashReportExample PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()

install(TARGETS asmCrashReportExample
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET asmCrashReportExample
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
