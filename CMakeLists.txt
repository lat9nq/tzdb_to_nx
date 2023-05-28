cmake_minimum_required(VERSION 3.10)

project(tzdb2nx VERSION 1.0)

set(TZDB_VERSION "$ENV{TZDB_VERSION}")
if ("${TZDB_VERSION}" STREQUAL "")
    set(TZDB_VERSION "2022c")
endif()

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
