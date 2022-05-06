#ifndef YQM_CENTRALCACHE_H
#define YQM_CENTRALCACHE_H
#include "PageCache.h"
#include "Util.h"

namespace YqmUtil::ymalloc
{

class CentralCache : YqmUtil::noncopyable
{

public:
    //获取一定数量的内存对象
    size_t GetNumObj(void *&start, void *&end, size_t num, size_t size);

    //将一定数量的对象释放到Page
    void ReleseNumObj(void *start, size_t size);
    static CentralCache *GetInstance()
    {
        static CentralCache *instance = new CentralCache();
        return instance;
    }

private:
    CentralCache() {}
    SpanList &GetList(size_t size);

    //从对应page获取span
    Span *GetSpanFromPage(size_t size);
private:
    SpanList _spanlists[FREE_LIST_NUM]; //构造慢
};

}
#endif
