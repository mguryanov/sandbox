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

#================================EXP-HTML-TABLES-FROM-PHP-SRC-TO-HHVM===========

include_directories("/home/maxim/projects/hhvm")

set(SRC_LIST_EHTPS_2_HHVM
    ehtps-2-hhvm.cpp
    zend-html.cpp
    zend-html.h
    zend-string.cpp
    zend-string.h
    test-html-entities.h
    html-table-php.h
    crypt-blowfish.c
    crypt-blowfish.h
)

add_executable(
    ehtps-2-hhvm
    ${SRC_LIST_EHTPS_2_HHVM}
)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -L/usr/lib/x86_64-linux-gnu/"
)

target_link_libraries(
    ehtps-2-hhvm
    -licuuc
    -lcrypt
)

#===============================================================================
