cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange/proto)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Package.cpp Package.h Protocols.h Protocols.cpp)

add_library(proto STATIC ${SOURCE_FILES})