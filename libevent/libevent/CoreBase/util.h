#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <stdio.h>
#ifdef _WIN32
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef uint16_t pluto_msgid_t;

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;

typedef float			float32_t;
typedef double			float64_t;
#endif // _WIN32


#ifndef GameAssert
#include <assert.h>
#if defined(_WIN32) || defined(_WIN64)
#pragma warning (disable:4996)
#define GameAssert(X,...)	do{if(!(X)){FILE* ff=fopen("assert.log","w");if(ff){fprintf(ff, "assert: [%s][%s][line:%d]!!!\n",__FILE__, __FUNCTION__, __LINE__);fprintf(ff, #X);/*fprintf(ff,":\n"__VA_ARGS__);*/fclose(ff);} abort();}}while(0)//assert(X) __asm { int 3 } //todotodotodo todotodotodotodo linux会有问题 测试下
#else
#define GameAssert(X,...)	do{if(!(X)){FILE* ff=fopen("assert.log","w");if(ff){fprintf(ff, "assert: [%s][%s][line:%d]!!!\n",__FILE__, __FUNCTION__, __LINE__);fprintf(ff, #X);/*fprintf(ff,":\n"__VA_ARGS__);*/fclose(ff);} abort();}}while(0)
#endif
#endif // !GameAssert

#endif

