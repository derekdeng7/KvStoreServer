cmake_minimum_required(VERSION 3.0.0)

project(test)

SET(CMAKE_CXX_COMPILER "g++")         #设置编译器（可选的有gcc，g++）

set( CMAKE_BUILD_TYPE Debug )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )

aux_source_directory(. DIR_SRCS)
add_executable(testout ${DIR_SRCS})


