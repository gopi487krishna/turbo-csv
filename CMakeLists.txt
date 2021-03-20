cmake_minimum_required(VERSION 3.0.0)
project(turbo-csv VERSION 0.1.0)

include(CTest)

# Specify Boost include path if vcpkg not avaiable
include_directories(
    /vcpkg/installed/x64-linux/include
)

# For Boost libraries path if vcpkg not available
link_directories(
    /vcpkg/installed/x64-linux/lib
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

enable_testing()

add_subdirectory(test)
