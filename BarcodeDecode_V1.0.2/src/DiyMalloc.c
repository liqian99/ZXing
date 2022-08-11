#include "DiyMalloc.h"

//内存管理模块
//当系统不支持动态内存分配时，采用自定义的内存池管理

///////////////////////////////////////////////////////////////////////////////
#if MEMORY_MODE==MEMORY_MODE_MEM_POOL


#else //#if MEMORY_MODE==MEMORY_MODE_MEM_POOL
	#undef diy_mem_pool_init
	#undef diy_mem_pool_clear
	#undef diy_malloc
	#undef diy_free
	void diy_mem_pool_init(void *mem_pool, int size) {}
	void diy_mem_pool_clear(void) {}
	#if MEMORY_MODE==MEMORY_MODE_STD_MALLOC
		void *diy_malloc(int size)
		{
			return malloc(size);
		}
		void diy_free(void *memblock)
		{
			if(memblock)
			   free(memblock);
		}
	#elif MEMORY_MODE==MEMORY_MODE_CPP_NEW_DEL
		void *diy_malloc(int size)
		{
			return new char[size];
		}
		void diy_free(void *memblock)
		{
			delete[] (char*)memblock;
		}
	#else
		#error unknown memory mode
	#endif
#endif //#if MEMORY_MODE==MEMORY_MODE_MEM_POOL

#if STD_MEMSET_COPY_MOVE==0
///////////////////////////////////////////////////////////////////////////////
void *diy_memset( void *dest, int c, int count )
{
    int i;
    unsigned char *ps;
    
    ps = (unsigned char*)dest;
    for (i = 0 ; i < count ; i++)
        *ps++ = (unsigned char)c;
    
    return (dest);
}

void *diy_memcpy( void *dest, const void *src, int count )
{
    int i;
    unsigned char *ps,*pd;
    ps = (unsigned char*)src;
    pd = (unsigned char*)dest;
    for (i = 0 ; i < count ; i++)
        *pd++ = *ps++;
        
    return (dest);
}

void *diy_memmove( void *dest, const void *src, int count )
{
    int i;
    unsigned char *ps,*pd;

    ps = (unsigned char*)src;
    pd = (unsigned char*)dest;
	if (src>dest)
	{
	    for (i = 0 ; i < count ; i++)
	       *pd++ = *ps++;
	}
	else
	{
		ps += count-1;
		pd += count-1;
	    for (i = 0 ; i < count ; i++)
	       *pd-- = *ps--;
	}

    return (dest);
}
#else //#if STD_MEMSET_COPY_MOVE
#undef diy_memset
#undef diy_memcpy
#undef diy_memmove
void *diy_memset( void *dest, int c, int count )
{
	return memset(dest,c,count);
}
void *diy_memcpy( void *dest, const void *src, int count )
{
	return memcpy(dest,src,count);
}
void *diy_memmove( void *dest, const void *src, int count )
{
	return memmove(dest,src,count);
}
#endif //#if STD_MEMSET_COPY_MOVE
