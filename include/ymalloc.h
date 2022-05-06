#include "ThreadCache.h"
#include "PageCache.h"
#include "CentralCache.h"

namespace YqmUtil::ymalloc
{

static void* ymalloc(size_t size)
{
	if( size < POOL_MAX_SIZE )    //[0-64kb] 走 thread cache
	{
		if( YThreadCache == nullptr )
			YThreadCache = new ThreadCache;
		return YThreadCache->yalloc(size);
	}
	else if( size <= ((PAGE_NUM_MAX-1) << PAGE_OFFSET) )    //[64kb-128kb] 走central cache
	{
		size_t align_size = _RoundUp(size,1 << PAGE_OFFSET);
		size_t pagenum = (align_size >> PAGE_OFFSET);
		Span* span = PageCache::GetInstance()->GetSpan(pagenum);
		span->listsize_ = align_size;
		void* ptr = (void*)(span->pageid_ << PAGE_OFFSET);
		return ptr;
	}
	else    //[128kb-->4GB]  走malloc
	{
		return malloc(size);
	}

}

static void yfree(void* ptr)
{
	//
	size_t pageid = (PAGEID)ptr >> PAGE_OFFSET;
	Span* span = PageCache::GetInstance()->IdtoSpan(pageid);
	if( span == nullptr )// [128*4kb,-]
	{
		free(ptr);
		return;
	}

	size_t size = span->listsize_;
	if( size <= POOL_MAX_SIZE ) // [1byte, 64kb]
	{
		YThreadCache->ydealloc(ptr,size);
	}
	else if( size <= ((PAGE_NUM_MAX-1) << PAGE_OFFSET) ) // (64kb, 128*4kb]
	{
		PageCache::GetInstance()->ReleaseSpan(span);
	}
}


}

