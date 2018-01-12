cmake_minimum_required(VERSION 3.8)
project(P2PFilesExchange/proto)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Package.cpp Package.h Protocols.cpp Protocols.h packages/ConnectPackage.cpp packages/ConnectPackage.h packages/KeepAlivePackage.cpp packages/KeepAlivePackage.h packages/DisconnectPackage.cpp packages/DisconnectPackage.h packages/HelloPackage.cpp packages/HelloPackage.h packages/ListPackage.cpp packages/ListPackage.h packages/DeadbodyPackage.cpp packages/DeadbodyPackage.h packages/PutPackage.cpp packages/PutPackage.h packages/DeletePackage.cpp packages/DeletePackage.h packages/GetPackage.cpp packages/GetPackage.h packages/NoFilePackage.cpp packages/NoFilePackage.h packages/FilePackage.cpp packages/FilePackage.h packages/OkPackage.cpp packages/OkPackage.h packages/AbortPackage.cpp packages/AbortPackage.h)

add_library(proto STATIC ${SOURCE_FILES})