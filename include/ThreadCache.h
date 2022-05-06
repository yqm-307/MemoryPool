#ifndef YQM_THREADCACHE_H
#define YQM_THREADCACHE_H

#include "Util.h"
#include "CentralCache.h"


namespace YqmUtil::ymalloc
{
    
class ThreadCache
{
public:
	ThreadCache(){

	}
	void* yalloc(size_t size);
	void ydealloc(void* ptr,size_t len);

private:
	//释放指定数量、大小的内存
	void ReleaseToCentralCache(FreeList& list,size_t num,size_t size);
	//从central cache 的span获取内存对象，获取内存对象数量由计算获得（越大给的越少）
	void* GetObjFromSpan(size_t index);
private:
	FreeList _freelist[FREE_LIST_NUM];
};

}
thread_local static YqmUtil::ymalloc::ThreadCache *YThreadCache = nullptr;

#endif