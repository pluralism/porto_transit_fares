cmake_minimum_required (VERSION 3.8)

project(andante_lib VERSION 1.0 DESCRIPTION "Andante Fares Lib")

set(CMAKE_PREFIX_PATH "C:\\Qt\\5.12.5\\msvc2017_64\\lib\\cmake")
find_package(Qt5Core CONFIG REQUIRED)
file(GLOB_RECURSE andante_lib_src "*.h" "*.cpp")
add_library(andante_lib STATIC ${andante_lib_src})

target_link_libraries(andante_lib Qt5::Core)
target_include_directories(andante_lib PUBLIC "andante_algorithms/include" "andante_manager/include" "andante_models/include")