#pragma once
#include <YqmUtil/Logger/Logger.h>
#include <YqmUtil/structure/List/DLinkList.h>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>

namespace YqmUtil::ymalloc
{

    //获取指针指向的值
    inline void *&Next(void *highptr)
    {
        return *((void **)highptr);
    }

    // free链表中，其实只保存一个节点。每个节点指向下一个节点
    class FreeList
    {
    public:
    FreeList() : size_(0), headnode(nullptr) {}
    size_t size()
    {
        return size_;
    }
    bool empty()
    {
        return headnode == nullptr;
    }
    void clear()
    {
        headnode = nullptr;
        size_ = 0;
    }

        //挂上一个节点
    void push(void *obj)
    {
        Next(obj) = headnode; // obj指向上一个节点
        headnode = obj;       // obj作为下一个插入的节点的上一个
        ++size_;              // size增长
    }

        //获取一个节点
    void *pop()
    {
        void *ret = headnode;
        headnode = Next(headnode);
        --size_;
        return ret;
    }

    //挂上n个节点
    void insert(void *begin, void *end, size_t num)
    {
        Next(end) = headnode;
        headnode = begin;
        size_ += num;
    }

        //获取n个节点
    size_t earse(void *&begin, void *&end, size_t num)
    {
        size_t delnum = 0;
        void *prev = nullptr;
        void *current = headnode;
        for (; current != nullptr && delnum < num; ++delnum)
        {
            prev = current;
            current = Next(current);
        }
        begin = headnode;
        end = prev;
        headnode = current;
        size_ -= delnum;
        return delnum;
    }

private:
    void *headnode;
    size_t size_;
};
}