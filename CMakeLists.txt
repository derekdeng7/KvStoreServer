cmake_minimum_required(VERSION 3.0.0)

project(KvStoreServer)

set(CMAKE_CXX_COMPILER "g++")

#This project require c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/leveldb
  ${PROJECT_SOURCE_DIR}/net
  ${PROJECT_SOURCE_DIR}/util
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "-g -pthread")

#add_subdirectory(leveldb)
add_subdirectory(net)
add_subdirectory(util)

add_executable(KvStoreServer ${PROJECT_SOURCE_DIR}/test/testServer.cpp)
target_link_libraries(KvStoreServer util_lib net_lib ${PROJECT_SOURCE_DIR}/leveldb/libleveldb.a)

add_executable(KvStoreClient ${PROJECT_SOURCE_DIR}/test/testClient.cpp)
target_link_libraries(KvStoreClient util_lib net_lib ${PROJECT_SOURCE_DIR}/leveldb/libleveldb.a)
