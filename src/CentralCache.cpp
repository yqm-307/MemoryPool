#include "../include/CentralCache.h"
#include "../include/Util.h"


namespace YqmUtil::ymalloc
{


Span* CentralCache::GetSpanFromPage(size_t size)
{
	SpanList& list = GetList(size);
	Span* node = list.begin();
	//1、当前页中是否有符合条件的span
	while( node != list.end() )
	{
		if( !node->freelist_.empty() )
			return node;
		else
			node = node->_next;
	}//如果没有

	//2、从系统申请
	Span* span = PageCache::GetInstance()->GetSpan(NumMovePage(size));
	//分割  计算偏移量
	char* start = (char*)(span->pageid_ << PAGE_OFFSET);
	char* end = start + (span->pagesize_ << PAGE_OFFSET);
	//把每个obj挂到freelist上
	while( start < end )
	{
		char* obj = start;
		start += size;
		span->freelist_.push(obj);
	}
	span->listsize_ = size;
	list.push_front(span);  //切割完毕的span挂到spanlist

	return span;
}


size_t CentralCache::GetNumObj(void*& start,void*& end,size_t num,size_t size)
{
	//1、获取指定大小内存到所在的list下标
	SpanList& list = GetList(size);
	size_t delnum = 0;
	//加锁，取出span
	list.Lock();
	Span* span = GetSpanFromPage(size);
	FreeList& freelist = span->freelist_;
	delnum = freelist.earse(start,end,num);
	span->count_ += delnum;
	list.Unlock();
	return delnum;
}


SpanList& CentralCache::GetList(size_t size)
{
	return _spanlists[ListIndex(size)];
}

//将size个obj释放到对应的Span
void CentralCache::ReleseNumObj(void* start,size_t size)
{
	SpanList& list = GetList(size);

	list.Lock();
	while( start )
	{
		// freelist head 节点后移
		void* next_ = Next(start);
		PAGEID id = (PAGEID)start >> PAGE_OFFSET;                              //计算页号
		Span* spannode = PageCache::GetInstance()->IdtoSpan(id); // obj 找到所对应的 span
		spannode->freelist_.push(start);                                      //  当前节点挂到freelist
		spannode->count_--;                                                   //  引用计数减一

		//如果obj对应的span也要被释放了
		if( spannode->count_ == 0 )
		{
			size_t index = ListIndex(spannode->listsize_);
			_spanlists[index].erase(spannode); //在spanlist中释放对应的span
			spannode->freelist_.clear();   //清空span对应的freelist

			PageCache::GetInstance()->ReleaseSpan(spannode); //释放空间
		}

		start = next_;
	}
	list.Unlock();
}





}