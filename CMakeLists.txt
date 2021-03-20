cmake_minimum_required(VERSION 3.0.0)
project(turbo-csv VERSION 0.1.0)

include(CTest)
INCLUDE_DIRECTORIES(
    /vcpkg/installed/x64-linux/include
)
LINK_DIRECTORIES(
    /vcpkg/installed/x64-linux/lib
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

enable_testing()

add_subdirectory(test)
