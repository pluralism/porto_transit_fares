cmake_minimum_required(VERSION 3.8)

project(andante_converter VERSION 1.0 DESCRIPTION "Andante format converter")

find_package(Qt5Core CONFIG REQUIRED)
add_library(andante_converter SHARED "include/converter.h" "converter.cpp")

target_link_libraries(andante_converter Qt5::Core)
target_link_libraries(andante_converter andante_lib)

target_include_directories(andante_converter PUBLIC include)