cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange)

find_package(Threads)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_subdirectory(src/conn)
add_subdirectory(src/ui)
add_subdirectory(src/files)
add_subdirectory(src/proto)
add_subdirectory(src/log)

add_executable(P2PFilesExchange src/main.cpp src/exception.cpp)
target_link_libraries(P2PFilesExchange
        conn ui files proto log
        ${CMAKE_THREAD_LIBS_INIT})