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
    "-std=c++11 -pedantic-errors -Wall -Wextra"
)



### astarta.m ###

set(src_astarta_m
    astarta-m.ru/astarta.m.cpp
)

set(src_astarta_m_tested
    astarta-m.ru/astarta.m.tested.cpp
)

add_executable(
    astarta-m.ru/astarta.m
    ${src_astarta_m}
)

add_executable(
    astarta-m.ru/astarta.m.tested
    ${src_astarta_m_tested}
)

add_custom_target(
    astarta.m_test
    SOURCES
    astarta-m.ru/astarta.m.test.pl
)

#add_custom_target(
#    astarta.m_test
#    SOURCES
#    rockstonedev.com/astarta.m.test.pl
#)


### rockstonedev.com ###

set(src_rockstonedev_task1
    rockstonedev.com/task1.cpp
)

set(src_rockstonedev_task2
    rockstonedev.com/task2.cpp
)

add_executable(
    rockstonedev.com/tic-tac-toe
    ${src_rockstonedev_task1}
)

add_executable(
    rockstonedev.com/refactoring
    ${src_rockstonedev_task2}
)

target_link_libraries(
    rockstonedev.com/refactoring -pthread
)
