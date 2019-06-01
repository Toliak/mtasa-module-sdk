#pragma once

#if defined( _M_X64 ) || defined( __x86_64__ ) || defined( _M_AMD64 )
#define ANY_x64
#ifdef _WIN64
#define WIN_x64
#else
#define LINUX_x64
#endif
#else
#define ANY_x86
    #ifdef WIN32
        #define WIN_x86
    #else
        #define LINUX_x86
    #endif
#endif

#ifdef __cplusplus
#define EXTERN_C extern "C"
#define EXTERN_C_BLOCK_START extern "C" {
#define EXTERN_C_BLOCK_END }
#else
#define EXTERN_C
#define EXTERN_C_BLOCK_START
#define EXTERN_C_BLOCK_END
#endif