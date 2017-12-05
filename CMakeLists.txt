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


set(TESTCSTRING_FILES
        src/cstring.h
        src/cstring.c
        src/unittest.h
        src/testcstring.c)


set(TESTMD5_FILES
        src/md5.h
        src/md5.c
        src/unittest.h
        src/testmd5.c)

set(TESTSHA1_FILES
        src/sha1.h
        src/sha1.c
        src/unittest.h
        src/testsha1.c)

add_executable(testarray ${TESTARRAY_FILES})
add_executable(testcstring ${TESTCSTRING_FILES})
add_executable(testmd5 ${TESTMD5_FILES})
add_executable(testsha1 ${TESTSHA1_FILES})