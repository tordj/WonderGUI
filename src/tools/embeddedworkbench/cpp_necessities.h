#ifndef CPP_NECESSITIES_DOT_H
#define CPP_NECESSITIES_DOT_H
#pragma once

#include <wg_c_memheap.h>

#ifdef __cplusplus
extern "C" {
#endif

        void    init_cpp_necessities(wg_memHeap heap); 
        void    exit_cpp_necessities(); 

#ifdef __cplusplus
}
#endif

#endif




