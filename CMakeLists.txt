cmake_minimum_required(VERSION 3.28)
project(Protokollitaja VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Network PrintSupport SerialPort Sql Test LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)
find_package(WrapIconv)

qt_standard_project_setup()


add_subdirectory(protokollitaja)
add_subdirectory(protolehelugeja)
add_subdirectory(protofinaal)
add_subdirectory(rangecontrol)
add_subdirectory(common)
