cmake_minimum_required(VERSION 3.5)

project(cc)

set(CMAKE_C_STANDARD 90)

set(TESTARRAY_FILES
        src/config.h
        src/color.h
        src/xmalloc.h
        src/xmalloc.c
        src/array.h
        src/array.c
        src/unittest.h
        src/testarray.c)

add_executable(testarray ${TESTARRAY_FILES})
