cmake_minimum_required(VERSION 3.1)
project(go-ai)

# Enable C++11
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-g -O2 -flto -static")

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

################################
# Main program
################################

file(GLOB go-ai_SRC
    "src/*.hpp"
    "src/*.cpp"
)
include_directories(src/)
add_executable(go-ai ${go-ai_SRC})

################################
# Unit Tests
################################
add_executable(gtplibTest ${gtplib_SOURCE_DIR}/test/test.cpp)
add_executable(board_test src/common/board_test.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(gtplibTest gtest gtest_main)
target_link_libraries(board_test gtest gtest_main)

add_test( gtplibTest gtplibTest )
add_test(board_test board_test)
