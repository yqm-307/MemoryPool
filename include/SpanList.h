
#include "FreeList.h"

//双链表的List

namespace YqmUtil::ymalloc
{
using namespace YqmUtil::Structure::List;
struct  Span
{
	PAGEID pageid_ = 0;
	int pagesize_ = 0;
	FreeList freelist_;
	size_t listsize_ = 0;     //所储存对象大小，即链表的级别

	int count_ = 0;
	Span* _next = nullptr;
	Span* _prev = nullptr;
};



class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	Span* begin()
	{
		return _head->_next;
	}

	Span* end()
	{
		return _head;
	}

	void push_front(Span* newspan)
	{
		Insert(_head->_next,newspan);
	}

	void pop_front()
	{
		erase(_head->_next);
	}

	void push_back(Span* newspan)
	{
		Insert(_head,newspan);
	}

	void pop_back()
	{
		erase(_head->_prev);
	}

	void Insert(Span* pos,Span* newspan)
	{
		Span* prev = pos->_prev;

		// prev newspan pos
		prev->_next = newspan;
		newspan->_next = pos;
		pos->_prev = newspan;
		newspan->_prev = prev;
	}

	void erase(Span* pos)
	{
		assert(pos != _head);

		Span* prev = pos->_prev;
		Span* next = pos->_next;

		prev->_next = next;
		next->_prev = prev;
	}

	bool empty()
	{
		return begin() == end();
	}

	void Lock()
	{
		_mtx.lock();
	}

	void Unlock()
	{
		_mtx.unlock();
	}

private:
	Span* _head;
	std::mutex _mtx;
};

}