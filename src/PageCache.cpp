#include "../include/PageCache.h"


namespace YqmUtil::ymalloc
{






//从某个page中获取一个span
Span* PageCache::GetPageFromSystem(size_t numpage)
{
	// 1、这一页非空，直接返回一个span
	if( !_pages[numpage].empty() )
	{
		Span* retspan = _pages[numpage].begin();
		_pages[numpage].pop_front();
		return retspan;
	}

	// 2、这一页为空，向更大的span中找一个非空的，切出一块儿
	for( size_t i = numpage + 1; i < PAGE_NUM_MAX; ++i )
	{
		if( !_pages[i].empty() )
		{
			// 分裂
			Span* span = _pages[i].begin();
			_pages[i].pop_front();

			Span* splitspan = new Span;
			splitspan->pageid_ = span->pageid_ + span->pagesize_ - numpage;
			splitspan->pagesize_ = numpage;
			for( PAGEID i = 0; i < numpage; ++i )
			{
				_idtospan[splitspan->pageid_ + i] = splitspan;
			}

			span->pagesize_ -= numpage;

			_pages[span->pagesize_].push_front(span);

			return splitspan;
		}
	}

	// 3、只能从系统中申请
	void* ptr = Alloc(PAGE_NUM_MAX - 1);

	Span* bigspan = new Span;
	bigspan->pageid_ = (PAGEID)ptr >> PAGE_OFFSET; //计算新的span的id
	bigspan->pagesize_ = PAGE_NUM_MAX - 1;
	for( PAGEID i = 0; i < PAGE_NUM_MAX - 1; ++i )
	{
		//_idSpanMap.insert(std::make_pair(bigspan->_pageid + i, bigspan));
		_idtospan[bigspan->pageid_ + i] = bigspan;
	}

	_pages[bigspan->pagesize_].push_front(bigspan); //加入

	return GetPageFromSystem(numpage); //再次尝试
}

Span* PageCache::GetSpan(size_t page_num)
{
	std::lock_guard<std::mutex> lock(_mutex);
	return GetPageFromSystem(page_num);
}

//自动合并上下页
void PageCache::ReleaseSpan(Span* span)
{
	//合并上一页
	while( 1 )
	{
		auto prev = _idtospan.find(span->pageid_ - 1);   //找到上一页

		if( prev != _idtospan.end() )
			break;

		Span* prevspan = prev->second;
		if( prevspan->count_ != 0 )
			break;
		if( span->pagesize_ + prevspan->pagesize_ >= PAGE_NUM_MAX )
			break;

		span->pageid_ = prevspan->pageid_;
		span->pagesize_ += prevspan->pagesize_;
		for( PAGEID i = 0; i < prevspan->pagesize_; ++i ) //归属权更改到上一页，其实就是内存紧缩
			_idtospan[prevspan->pageid_ + i] = span;
		_pages[prevspan->pagesize_].erase(prev->second);
		delete prevspan;
	}

	//合并下一页
	while( 1 )
	{
		auto next = _idtospan.find(span->pageid_ + span->pagesize_);
		if( next == _idtospan.end() )
			break;
		Span* nextSpan = next->second;
		if( nextSpan->count_ != 0 )
			break;
		if( span->pagesize_ + nextSpan->pagesize_ >= PAGE_NUM_MAX )
			break;


		span->pagesize_ += nextSpan->pagesize_;
		for( PAGEID i = 0; i < nextSpan->pagesize_; ++i )
			_idtospan[nextSpan->pageid_ + i] = span;
		_pages[nextSpan->pagesize_].erase(nextSpan);
		delete nextSpan;
	}

	_pages[span->pagesize_].push_front(span);   //重新插入回去

}

Span* PageCache::operator[](PAGEID id)
{
	auto p = _idtospan.find(id);
	if( p == _idtospan.end() )
		return nullptr;
	else
		return p->second;
}

Span* PageCache::IdtoSpan(size_t spanid)
{
	auto p = _idtospan.find(spanid);
	if( p != _idtospan.end() )
		return p->second;
	else
		return nullptr;
}










}
