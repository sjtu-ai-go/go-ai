cmake_minimum_required(VERSION 3.1)
project(go-ai)

# Enable C++11
set (CMAKE_CXX_STANDARD 11)

enable_testing()
################################
# GTest
################################
ADD_SUBDIRECTORY (vendor/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# gtplib
################################

set(gtplib_SOURCE_DIR vendor/gtplib)
include_directories(${gtplib_SOURCE_DIR})
add_executable(gtplibTest ${gtplib_SOURCE_DIR}/test/test.cpp)

################################
# Unit Tests
################################
# Link test executable against gtest & gtest_main
target_link_libraries(gtplibTest gtest gtest_main)
add_test( gtplibTest gtplibTest )
