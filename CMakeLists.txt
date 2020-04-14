# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
project ("andante_fares")

# Include sub-projects.
add_subdirectory ("andante_lib")
add_subdirectory ("andante_sandbox")
add_subdirectory ("andante_converter")