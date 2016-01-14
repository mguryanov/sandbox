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

#================================FRAGMEM========================================

set(SRC_LIST_FRAGMEM
    fragmem.c
)

add_executable(
    fragmem
    ${SRC_LIST_FRAGMEM}
)

#================================MEMALLOC-CAST-DISADV===========================

set(SRC_LIST_MEMALLOC_CAST_DISADV
    malloc-cast-disadv.c
)

add_executable(
    malloc-cast-disadv
    ${SRC_LIST_MEMALLOC_CAST_DISADV}
)

#================================MEMALLOC-CAST-ADV==============================

set(SRC_LIST_MEMALLOC_CAST_ADV
    malloc-cast-adv.cpp
)

add_executable(
    malloc-cast-adv
    ${SRC_LIST_MEMALLOC_CAST_ADV}
)

#================================MALLOC-BUG=====================================

set(SRC_LIST_MALLOC_BUG
    malloc-bug.c
)

add_executable(
    malloc-bug
    ${SRC_LIST_MALLOC_BUG}
)

#================================MALLOC-NULL====================================

set(SRC_LIST_MALLOC_NULL
    malloc-null.c
)

add_executable(
    malloc-null
    ${SRC_LIST_MALLOC_NULL}
)

#================================GDB-MOCK=======================================

set(SRC_LIST_GDB_MOCK
    gdb-mock.cpp
)

add_executable(
    gdb-mock
    ${SRC_LIST_GDB_MOCK}
)

#================================COPY-CTOR======================================

set(SRC_LIST_COPY_CTOR
    copy-ctor.cpp
)

add_executable(
    copy-ctor
    ${SRC_LIST_COPY_CTOR}
)

#================================MULTI-CODEPOINT-TABLE-HHVM=====================

set(SRC_LIST_MCPT_HHVM
    mcpt-hhvm.cpp
    html-table.cpp
    html-table.h
)

add_executable(
    mcpt-hhvm
    ${SRC_LIST_MCPT_HHVM}
)

#================================LESSONS-ALGORITHMS=============================

### Lesson 1 ###

set(SRC_LIST_ALG_L1E1
    lessons-algorithms/l1e1.cpp
)

add_executable(
    algorithms/l1e1
    ${SRC_LIST_ALG_L1E1}
)

### Ex 2 ###

set(SRC_LIST_ALG_L1E2
    lessons-algorithms/l1e2.cpp
)

add_executable(
    algorithms/l1e2
    ${SRC_LIST_ALG_L1E2}
)

### Ex 2-1 ###

set(SRC_LIST_ALG_L1E2_1
    lessons-algorithms/l1e2-1.cpp
)

add_executable(
    algorithms/l1e2-1
    ${SRC_LIST_ALG_L1E2_1}
)

### Ex 2-2 ###

set(SRC_LIST_ALG_L1E2_2
    lessons-algorithms/l1e2-2.cpp
)

add_executable(
    algorithms/l1e2-2
    ${SRC_LIST_ALG_L1E2_2}
)

### Ex 3 ###

set(SRC_LIST_ALG_L1E3
    lessons-algorithms/l1e3.cpp
)

add_executable(
    algorithms/l1e3
    ${SRC_LIST_ALG_L1E3}
)



### Lesson 2 ###

### Ex 1 ###

set(SRC_LIST_ALG_L2E1
    lessons-algorithms/l2e1.cpp
)

add_executable(
    algorithms/l2e1
    ${SRC_LIST_ALG_L2E1}
)

### Ex 2 ###

set(SRC_LIST_ALG_L2E2
    lessons-algorithms/l2e2.cpp
)

add_executable(
    algorithms/l2e2
    ${SRC_LIST_ALG_L2E2}
)

### Ex 3 ###

set(SRC_LIST_ALG_L2E3
    lessons-algorithms/l2e3.cpp
)

add_executable(
    algorithms/l2e3
    ${SRC_LIST_ALG_L2E3}
)



### Lesson 3 ###

### Ex 1 ###

set(SRC_LIST_ALG_L3E1
    lessons-algorithms/l3e1.cpp
)

add_executable(
    algorithms/l3e1
    ${SRC_LIST_ALG_L3E1}
)

### Ex 2 ###

### Ex 3 ###

#================================LESSONS-MULTITHREADING=========================

### Lesson 1 ###

### Ex 1 ###

#set(SRC_LIST_MTH_L1E1
#    lessons-multithreading/l1e1.cpp
#)

#add_executable(
#    multithreading/l1e1
#    ${SRC_LIST_MTH_L1E1}
#)

#================================LESSONS-BASE-CPP===============================

### Lesson 1 ###

### Ex 1 ###

set(SRC_LIST_BASE_L1E1
    lessons-base-cpp/l1e1.cpp
)

add_executable(
    base/l1e1
    ${SRC_LIST_BASE_L1E1}
)

### Ex 2 ###

set(SRC_LIST_BASE_L1E2
    lessons-base-cpp/l1e2.cpp
)

add_executable(
    base/l1e2
    ${SRC_LIST_BASE_L1E2}
)

### Ex 3 ###

set(SRC_LIST_BASE_L1E3
    lessons-base-cpp/l1e3.cpp
)

add_executable(
    base/l1e3
    ${SRC_LIST_BASE_L1E3}
)

### Ex 4 ###

set(SRC_LIST_BASE_L1E4
    lessons-base-cpp/l1e4.cpp
)

add_executable(
    base/l1e4
    ${SRC_LIST_BASE_L1E4}
)

### Ex 5 ###

set(SRC_LIST_BASE_L1E5
    lessons-base-cpp/l1e5.cpp
)

add_executable(
    base/l1e5
    ${SRC_LIST_BASE_L1E5}
)

### Ex 6 ###

set(SRC_LIST_BASE_L1E6
    lessons-base-cpp/l1e6.cpp
)

add_executable(
    base/l1e6
    ${SRC_LIST_BASE_L1E6}
)

### Ex 7 ###

set(SRC_LIST_BASE_L1E7
    lessons-base-cpp/l1e7.cpp
)

add_executable(
    base/l1e7
    ${SRC_LIST_BASE_L1E7}
)

#===============================================================================
