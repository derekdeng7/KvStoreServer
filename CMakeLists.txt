cmake_minimum_required(VERSION 3.0.0)

project(KvStoreServer)

set(CMAKE_CXX_COMPILER "g++")

#This project require c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BUILD_TYPE Debug)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/db
  ${PROJECT_SOURCE_DIR}/net
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "-g -Wall -pthread")

add_subdirectory(db)
add_subdirectory(net)

add_executable(KvStoreServer ${PROJECT_SOURCE_DIR}/test/testServer.cpp)
target_link_libraries(KvStoreServer net_lib db_lib)

add_executable(KvStoreClient ${PROJECT_SOURCE_DIR}/test/client.cpp)

