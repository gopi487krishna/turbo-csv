cmake_minimum_required(VERSION 3.10)
project(turbo-csv VERSION 0.1.0 LANGUAGES CXX)

include(CTest)


#Set C++20 standard

if("9.3.0" STREQUAL ${CMAKE_CXX_COMPILER_VERSION})
set(set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a"))
else()
set(CMAKE_CXX_STANDARD 20)
endif()


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_definitions(-DEXAMPLES="${CMAKE_CURRENT_SOURCE_DIR}/test/test_files/")

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

# Gets the include directory of boost (Base Path)
find_path(BOOST_INCLUDE_DIR boost/test/unit_test.hpp)

# Adds the base path to the include_directories ( All boost headers can be used now)
include_directories(${BOOST_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

add_subdirectory(test)

if(DEFINED ENV{GENERATE_BENCHMARK})
    find_package(benchmark CONFIG REQUIRED)
    add_subdirectory(benchmark) 
endif()

