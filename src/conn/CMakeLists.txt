cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange/conn)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES IPv4Address.cpp)

add_library(conn STATIC ${SOURCE_FILES})