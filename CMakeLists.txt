cmake_minimum_required(VERSION 4.3)
project(2MBC30_Path_Traversal)

set(CMAKE_CXX_STANDARD 20)

add_library(path_traversal_core
        modules/normaliser.cpp
        modules/normaliser.h
        classes/Node.cpp
        classes/Node.h
        modules/compactor.cpp
        modules/compactor.h
        classes/Stroke.cpp
        classes/Stroke.h
        classes/Pattern.cpp
        classes/Pattern.h
        patterns/digits.cpp
        patterns/digits.h
        modules/detector.cpp
        modules/detector.h
        modules/tracer.cpp
        modules/tracer.h)
target_include_directories(path_traversal_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(2MBC30_Path_Traversal main.cpp)
target_link_libraries(2MBC30_Path_Traversal PRIVATE path_traversal_core)

include(FetchContent)
FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(tests
        tests/helpers.h
        tests/stroke_test.cpp
        tests/pattern_test.cpp
        tests/two_test.cpp)
target_link_libraries(tests PRIVATE path_traversal_core GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)
