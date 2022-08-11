#ifndef __DIY_MALLOC_H__
#define __DIY_MALLOC_H__

#include "BaseTypeDef.h"

///////////////////////////////////////////////////////////////////////////////
//配置内存管理模式
#define MEMORY_MODE_STD_MALLOC		1	//使用系统内存分配库函数malloc和free
#define MEMORY_MODE_MEM_POOL		2	//使用内存池
#define MEMORY_MODE_CPP_NEW_DEL		3	//使用c++语言的new和delete,C语言不支持

#define MEMORY_MODE MEMORY_MODE_STD_MALLOC

///////////////////////////////////////////////////////////////////////////////
//内存池相关函数声明
#if MEMORY_MODE==MEMORY_MODE_MEM_POOL
void diy_mem_pool_init(void *mem_pool, int size);//初始化内存池
void diy_mem_pool_clear(void);	//释放从内存池中分配的所有内存空间
void *diy_malloc(int size);		//从内存池分配一块内存
void diy_free(void *memblock);	//释放一块从内存池分配的内存
void *diy_realloc(void *memblock,int size);
#elif MEMORY_MODE==MEMORY_MODE_STD_MALLOC
#include <stdlib.h>
#define diy_mem_pool_init(mem_pool,size)
#define diy_mem_pool_clear()
#define diy_malloc(size) malloc(size)
#define diy_free(memblock) free(memblock)
#define diy_realloc(p,size) realloc(p,size)
#elif MEMORY_MODE == MEMORY_MODE_CPP_NEW_DEL
#define diy_mem_pool_init(mem_pool,size)
#define diy_mem_pool_clear()
#define diy_malloc(size) ((void *)new char[size])
#define diy_free(memblock) (delete[] memblock)
#else
//#error unknown memory mode
#endif //#if MEMORY_MODE==MEMORY_MODE_MEM_POOL

#define OUT_OF_MEMORY           -2005
#define MALLOC(X,type,size) { X=(type*)diy_malloc(((size)*sizeof(type))); if(X==NULL) { ret=OUT_OF_MEMORY; goto ERROR_EXIT; } else {diy_memset(X,0x00,((size)*sizeof(type)));} }
#define FREE(X) if (X!=NULL) { diy_free(X); X=NULL; }
#define REALLOC(X,type,size) {X=(type*)diy_realloc(X,((size)*sizeof(type)));if(X==NULL) { ret=OUT_OF_MEMORY; goto ERROR_EXIT;} else {diy_memset(X,0x00,((size)*sizeof(type)));}}

///////////////////////////////////////////////////////////////////////////////
//系统是否支持memset,memcpy,memmove三个函数。0为不支持，1为支持。
#define STD_MEMSET_COPY_MOVE	1

#if STD_MEMSET_COPY_MOVE
#include <string.h>
#define diy_memset(dest,c,count)		memset(dest,c,count)
#define	diy_memcpy(dest,src,count)	memcpy(dest,src,count)
#define diy_memmove(dest,src,count)	memmove(dest,src,count)
#else
void *diy_memset( void *dest, int c, int count );
void *diy_memcpy( void *dest, const void *src, int count );
void *diy_memmove( void *dest, const void *src, int count );
#endif

#endif /*__DIY_MALLOC_H__*/