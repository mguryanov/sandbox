project(sandbox)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE
    ON
)

set(CMAKE_BUILD_TYPE
    Debug
)

set(CMAKE_C_FLAGS
    "-std=c99 -pedantic-errors"
)

set(CMAKE_CXX_FLAGS
    "-std=c++11 -pedantic-errors"
)

add_subdirectory( hhvm )
