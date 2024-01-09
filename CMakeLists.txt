cmake_minimum_required(VERSION 3.10)

project(tzdb2nx VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    find_package(Intl REQUIRED)
endif()

add_subdirectory(externals)
add_subdirectory(src)
