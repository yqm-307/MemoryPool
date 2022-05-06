#include "../include/ThreadCache.h"

#define GetList(size) _freelist[ListIndex(size)]

namespace YqmUtil::ymalloc
{

#define GetList(size) _freelist[ListIndex(size)]



//从central cache中获取内存，然后返回一个
void* ThreadCache::GetObjFromSpan(size_t size)
{
	// 1、从central获取
	size_t objnum = NumMoveSize(size);
	void* begin = nullptr;
	void* end = nullptr;
	size_t delnum = CentralCache::GetInstance()->GetNumObj(begin,end,objnum,size);

	// 2、保存一下多余的（如果有的话
	//只有一个，就不需要挂到freelist中，直接返回
	if( delnum == 1 )
	{
		return begin;
	}
	else
	{ //有多个，返回一个，其他的挂在freelist上
		FreeList& list = GetList(size);

		list.insert(Next(begin),end,delnum - 1);
		return begin;
	}
}

void* ThreadCache::yalloc(size_t size)
{
	FreeList& list = GetList(size);
	//对应对象链表如果非空，直接返回
	if( !list.empty() )
		return list.pop();
	else
	{                                                  //否则向central cache中申请
		return this->GetObjFromSpan(RoundUp(size)); //为什么roundup，因为对齐减少内碎片，所以不能直接传我们需要内存大小，通过roundup计算合适的obj大小
	}
}

void ThreadCache::ydealloc(void* ptr,size_t size)
{
	//挂回对应的节点
	FreeList& list = GetList(size);
	list.push(ptr);

	//获取对应链表最合适容量
	size_t maxlistsize = NumMoveSize(size);
	if( list.size() >= maxlistsize )
	{//需要归还内存
		ReleaseToCentralCache(list,maxlistsize,size);
	}

}

void ThreadCache::ReleaseToCentralCache(FreeList& list,size_t num,size_t size)
{
	//1、多余部分切除
	void* begin = nullptr;
	void* end = nullptr;
	list.earse(begin,end,num);
	if( end != nullptr )
		Next(end) = nullptr;
	//2、归还
	CentralCache::GetInstance()->ReleseNumObj(begin,size);
}


#undef GetList(size)



}

#undef GetList(size)

