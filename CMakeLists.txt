cmake_minimum_required(VERSION 3.0.0)
project(turbo-csv VERSION 0.1.0)

include(CTest)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Gets the include directory of boost (Base Path)
find_path(BOOST_INCLUDE_DIR boost/test/unit_test.hpp)

# Adds the base path to the include_directories ( All boost headers can be used now)
include_directories(${BOOST_INCLUDE_DIR})

enable_testing()

add_subdirectory(test)
