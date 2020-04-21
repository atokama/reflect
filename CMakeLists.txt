cmake_minimum_required(VERSION 3.10)

project(reflect)

# Lib RTTR configuration
set(BUILD_UNIT_TESTS OFF CACHE BOOL "")
set(BUILD_BENCHMARKS OFF CACHE BOOL "")
set(BUILD_EXAMPLES OFF CACHE BOOL "")
set(BUILD_DOCUMENTATION OFF CACHE BOOL "")
set(BUILD_INSTALLER OFF CACHE BOOL "")
set(BUILD_PACKAGE OFF CACHE BOOL "")
set(USE_PCH OFF CACHE BOOL "")
set(CUSTOM_DOXYGEN_STYLE OFF CACHE BOOL "")
set(BUILD_WEBSITE_DOCU OFF CACHE BOOL "")

add_subdirectory(../rttr rttr)

find_package(RapidJSON REQUIRED)

find_package(GTest REQUIRED)

add_executable(reflect_test
    test/gtest_main.cpp
    test/rttr_test.cpp)

target_include_directories(reflect_test PUBLIC ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(reflect_test rttr_core gtest)

