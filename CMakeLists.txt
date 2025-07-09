cmake_minimum_required(VERSION 3.22)
project(mdc)

set(CMAKE_CXX_STANDARD 23)

add_executable(mdc src/mdc.cpp src/libmain.h src/markdown.h src/mdc.cpp)
add_executable(mdc_tests src/tests.cpp src/libmain.h src/markdown.h)