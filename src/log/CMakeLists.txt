cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange/ui)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Logger.cpp Logger.h)

add_library(log STATIC ${SOURCE_FILES})