

#ifndef __CCSTATE__H__INCLUDE__
#define __CCSTATE__H__INCLUDE__


#include "config.h"


enum compiler_output_type_e {
    COMPILER_OUTPUT_MEMORY       = 0,
    COMPILER_OUTPUT_EXE          = 1,
    COMPILER_OUTPUT_DLL          = 2,
    COMPILER_OUTPUT_OBJ          = 3,
    COMPILER_OUTPUT_PREPROCESS   = 4,
} compiler_output_type_t;


struct compiler_s {
    compiler_output_type_t output_type;

} compiler_t;


#endif
