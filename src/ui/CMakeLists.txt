cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange/ui)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES UI.cpp UI.h)

add_library(ui STATIC ${SOURCE_FILES})