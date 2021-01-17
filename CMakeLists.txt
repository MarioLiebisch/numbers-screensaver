cmake_minimum_required(VERSION 3.15)
project(numbers)

find_package(SFML 2.5 COMPONENTS graphics window system main REQUIRED)

set(CMAKE_EXECUTABLE_SUFFIX ".scr")
add_executable(numbers WIN32 numbers.cpp resources.rc)
set_property(TARGET numbers PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(TARGET numbers PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(numbers sfml-graphics sfml-window sfml-system sfml-main)
