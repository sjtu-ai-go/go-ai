cmake_minimum_required(VERSION 3.1)
project(go-ai CXX)

# Enable C++11
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-g -O2 -flto")

enable_testing()

################################
# Find boost
################################
find_package(Boost 1.52 REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

################################
# Find thread
################################
find_package(Threads REQUIRED)

################################
# GTest
################################
ADD_SUBDIRECTORY (vendor/googletest/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# gtplib
################################

set(gtplib_SOURCE_DIR vendor/gtplib)
find_package(Boost REQUIRED)
include_directories(${gtplib_SOURCE_DIR})

################################
# Spdlog
################################
set(spdlog_SOURCE_DIR vendor/spdlog)
add_subdirectory (${spdlog_SOURCE_DIR})
include_directories(${spdlog_SOURCE_DIR}/include)

################################
# CompressedGrid
################################
set(compressedgrid_SOURCE_DIR vendor/CompressedGrid)
include_directories(${compressedgrid_SOURCE_DIR})

################################
# SRCS
################################
set(board_SRC src/common/board/basic.hpp src/common/board/board_class.hpp src/common/board/board_grid.hpp
        src/common/board/grid_point.hpp src/common/board/group_node.hpp src/common/board/pos_group.hpp
        src/common/board.hpp)

set(logger_SRC src/common/logger.cpp src/common/logger.hpp)

set(common_SRC ${board_SRC} ${logger_SRC})

set(engines_SRC src/engines/example_engine.hpp src/engines/example_engine.cpp src/engines/engine_factory.hpp)

################################
# Main program
################################
include_directories(src/)
add_executable(go-ai ${common_SRC} ${engines_SRC} src/main.cpp)
target_link_libraries(go-ai ${Boost_PROGRAM_OPTIONS_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

################################
# Unit Tests
################################
add_executable(gtplibTest ${gtplib_SOURCE_DIR}/test/test.cpp)
add_executable(board_test src/common/board_test.cpp src/common/board/board_grid.hpp src/common/board/grid_point.hpp src/common/board/basic.hpp src/common/board/group_node.hpp src/common/board/pos_group.hpp src/common/board/board_class.hpp)
add_executable(logger_test src/common/logger_test.cpp src/common/logger.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(gtplibTest gtest gtest_main)
target_link_libraries(board_test gtest gtest_main)
target_link_libraries(logger_test gtest gtest_main)

add_test( gtplibTest gtplibTest )
add_test(board_test board_test)
add_test(logger_test logger_test)
