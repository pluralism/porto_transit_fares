cmake_minimum_required (VERSION 3.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5Core CONFIG REQUIRED)

add_executable (andante_sandbox "main.cpp")
target_link_libraries(andante_sandbox Qt5::Core)
target_link_libraries(andante_sandbox andante_converter)
target_link_libraries(andante_sandbox andante_lib)