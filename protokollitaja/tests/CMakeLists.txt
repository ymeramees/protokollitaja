cmake_minimum_required(VERSION 3.16)
project(tests VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()


add_subdirectory(CompetitorTest)
add_subdirectory(DataUploaderTest)
add_subdirectory(FinalsFileExportTest)
add_subdirectory(InitialDialogTest)
add_subdirectory(KllFileRWTest)
add_subdirectory(SheetTest)
add_subdirectory(StartListWriterTest)
add_subdirectory(UpdateCheckerTest)
