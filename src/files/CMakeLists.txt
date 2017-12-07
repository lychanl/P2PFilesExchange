cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange/files)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES FileManager.h FileManager.cpp)

add_library(files STATIC ${SOURCE_FILES})