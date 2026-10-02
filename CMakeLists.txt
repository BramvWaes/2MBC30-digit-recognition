cmake_minimum_required(VERSION 4.3)
project(2MBC30_Path_Traversal)

set(CMAKE_CXX_STANDARD 20)

add_executable(2MBC30_Path_Traversal main.cpp
        modules/normaliser.cpp
        modules/normaliser.h)
