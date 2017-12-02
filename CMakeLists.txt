cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_subdirectory(src/conn)
add_subdirectory(src/ui)

add_executable(P2PFilesExchange src/main.cpp src/exception.cpp)
target_link_libraries(P2PFilesExchange
        conn ui)