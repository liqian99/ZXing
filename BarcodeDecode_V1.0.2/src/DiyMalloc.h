#ifndef __DIY_MALLOC_H__
#define __DIY_MALLOC_H__

#include "BaseTypeDef.h"

///////////////////////////////////////////////////////////////////////////////
//�����ڴ����ģʽ
#define MEMORY_MODE_STD_MALLOC		1	//ʹ��ϵͳ�ڴ����⺯��malloc��free
#define MEMORY_MODE_MEM_POOL		2	//ʹ���ڴ��
#define MEMORY_MODE_CPP_NEW_DEL		3	//ʹ��c++���Ե�new��delete,C���Բ�֧��

#define MEMORY_MODE MEMORY_MODE_STD_MALLOC

///////////////////////////////////////////////////////////////////////////////
//�ڴ����غ�������
#if MEMORY_MODE==MEMORY_MODE_MEM_POOL
void diy_mem_pool_init(void *mem_pool, int size);//��ʼ���ڴ��
void diy_mem_pool_clear(void);	//�ͷŴ��ڴ���з���������ڴ�ռ�
void *diy_malloc(int size);		//���ڴ�ط���һ���ڴ�
void diy_free(void *memblock);	//�ͷ�һ����ڴ�ط�����ڴ�
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
//ϵͳ�Ƿ�֧��memset,memcpy,memmove����������0Ϊ��֧�֣�1Ϊ֧�֡�
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