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

#================================MULTI-CODEPOINT-TABLE-HHVM=====================

set(SRC_LIST_MCPT_HHVM
    hhvm/mcpt-hhvm.cpp
    hhvm/html-table.cpp
    hhvm/html-table.h
)

add_executable(
    hhvm/mcpt-hhvm
    ${SRC_LIST_MCPT_HHVM}
)

#================================EXP-HTML-TABLES-FROM-PHP-SRC-TO-HHVM===========

include_directories("/home/maxim/projects/hhvm")

set(SRC_LIST_EHTPS_2_HHVM
    hhvm/ehtps-2-hhvm.cpp
    hhvm/zend-html.cpp
    hhvm/zend-html.h
    hhvm/zend-string.cpp
    hhvm/zend-string.h
    hhvm/test-html-entities.h
    hhvm/html-table-php.h
    hhvm/crypt-blowfish.c
    hhvm/crypt-blowfish.h
)

add_executable(
    hhvm/ehtps-2-hhvm
    ${SRC_LIST_EHTPS_2_HHVM}
)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -L/usr/lib/x86_64-linux-gnu/"
)

target_link_libraries(
    hhvm/ehtps-2-hhvm
    -licuuc
    -lcrypt
)

#===============================================================================
