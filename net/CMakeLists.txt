cmake_minimum_required(VERSION 3.0.0)

project(KvStoreServer)

set(CMAKE_CXX_COMPILER "g++")         

set( CMAKE_BUILD_TYPE Debug )

set( CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -pthread" )

aux_source_directory(. DIR_SRCS)
add_executable(KvStoreServer ${DIR_SRCS})


