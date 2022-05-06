#pragma once
#include <YqmUtil/noncopyable/noncopyable.h>
#include <unordered_map>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

namespace YqmUtil::ymalloc
{

#define POOL_MAX_SIZE (64*1024)  //64KB
#define PAGE_SIZE 4096      //4KB
#define PAGE_NUM_MAX 129    //128页
#define FREE_LIST_NUM (POOL_MAX_SIZE/8)  //8192
#define PAGE_OFFSET 12  //页号偏移量
typedef uint64_t PAGEID;




// 控制在[1%，10%]左右的内碎片浪费
// [1,128] 8byte对齐 freelist[0,16)
// [129,1024] 16byte对齐 freelist[16,72)
// [1025,8*1024] 128byte对齐 freelist[72,128)
// [8*1024+1,64*1024] 1024byte对齐 freelist[128,1024)
static size_t _RoundUp(size_t size,size_t alignment)
{
	return (size + alignment - 1) & (~(alignment - 1));
}

// [9-16] + 7 = [16-23] -> 16 8 4 2 1
// [17-32] + 15 = [32,47] ->32 16 8 4 2 1

//对齐大小的计算，然后把申请来的内存进行划分
static inline size_t RoundUp(size_t size)
{
	assert(size <= POOL_MAX_SIZE);
	if( size <= 128 )
	{
		return _RoundUp(size,8);
	}
	else if( size <= 1024 )
	{
		return _RoundUp(size,16);
	}
	else if( size <= 8192 )
	{
		return _RoundUp(size,128);
	}
	else if( size <= 65536 )
	{
		return _RoundUp(size,1024);
	}

	return -1;
}

// [9-16] + 7 = [16-23]
#define _ListIndex(size, align_shift) (((size + (1 << align_shift) - 1) >> align_shift) - 1)

//映射自由链表的位置
static size_t ListIndex(size_t size)
{
	assert(size <= POOL_MAX_SIZE);

	// 每个区间有多少个链
	static int group_array[4] = { 16, 56, 56, 56 };
	if( size <= 128 )
	{
		return _ListIndex(size,3);
	}
	else if( size <= 1024 )
	{
		return _ListIndex(size - 128,4) + group_array[0];
	}
	else if( size <= 8192 )
	{
		return _ListIndex(size - 1024,7) + group_array[1] + group_array[0];
	}
	else if( size <= 65536 )
	{
		return _ListIndex(size - 8192,10) + group_array[2] + group_array[1] + group_array[0];
	}

	return -1;
}

//申请空间越小，返回块数量越多
static size_t NumMoveSize(size_t size)
{
	if( size == 0 )
		return 0;

	int num = POOL_MAX_SIZE / size;
	if( num < 2 )
		num = 2;

	if( num > 512 )
		num = 512;

	return num;
}

//计算一次向系统获取几个页
static size_t NumMovePage(size_t size)
{
	size_t num = NumMoveSize(size);
	size_t npage = num * size;

	npage >>= 12;
	if( npage == 0 )
		npage = 1;

	return npage;
}

inline static void* Alloc(int pagenum)
{
    //todo brk
    return malloc(pagenum*(PAGE_SIZE));
}

inline static void DeAlloc(void* ptr)
{
    //todo brk
    if(ptr == nullptr)
        throw std::exception();
    else
        free(ptr);
}



}


#include "FreeList.h"
#include "SpanList.h"
