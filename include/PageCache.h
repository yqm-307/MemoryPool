#ifndef YQM_PAGECACHE_H
#define YQM_PAGECACHE_H
#include "Util.h"
namespace YqmUtil::ymalloc
{
class PageCache :YqmUtil::noncopyable
{
public:
	static PageCache* GetInstance()
	{
		static PageCache* instance = new PageCache();
		return instance;
	}

	//从指定page中获取span
	Span* GetSpan(size_t page_num);

	//获取某块Span
	Span* IdtoSpan(size_t spanid);

	//释放span
	void ReleaseSpan(Span* span);
private:
	PageCache() :_pages(){}
	Span* operator[](PAGEID id);
	//获取span，或许从系统申请
	Span* GetPageFromSystem(size_t num);
private:
	SpanList _pages[PAGE_NUM_MAX];
	std::unordered_map<PAGEID,Span*> _idtospan;
	std::mutex _mutex;
};
}
#endif