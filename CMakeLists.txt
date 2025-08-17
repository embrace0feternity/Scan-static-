cmake_minimum_required(VERSION 3.25)
project(scan VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(target scan)

find_package(GTest REQUIRED)

add_library(${target} INTERFACE)

target_include_directories(${target} INTERFACE include/)

set(test_target scan_tests)

add_executable(${test_target} tests/main.cpp)
target_compile_options(${test_target} PUBLIC -std=gnu++2c -fno-rtti)
get_target_property(EXTENSIONS ${test_target} COMPILE_OPTIONS)
message(STATUS "Flag-extensions ${EXTENSIONS}")
target_link_libraries(${test_target} PRIVATE ${target})
target_link_libraries(${test_target} PRIVATE GTest::GTest GTest::Main)
