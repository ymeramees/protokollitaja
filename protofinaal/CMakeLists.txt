cmake_minimum_required(VERSION 3.16)
project(protofinaal VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()


add_subdirectory(protofinaal)
add_subdirectory(tests)
