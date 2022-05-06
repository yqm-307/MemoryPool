#include "include/ymalloc.h"
#include <YqmUtil/timer/interval.hpp>
#include <vector>
#include <atomic>

using namespace YqmUtil::ymalloc;

void BenchmarkYMalloc()
{
	std::atomic_int usetime(0);
	std::atomic_int overnum(0);
	for (int i = 0; i < 10; ++i)
		std::thread([&usetime, &overnum]()
					{
		int* p[10000]={0};
		clock_t begin = std::chrono::system_clock::now().time_since_epoch().count();
		for(int j=0;j<10;++j)
		{for(int i=0;i<10000;++i)
			p[i] = (int*)ymalloc(40);
		
			for(int i=0;i<10000;++i)
			yfree(p[i]);
		}
		clock_t end = std::chrono::system_clock::now().time_since_epoch().count();
		usetime += (end-begin)/1000/1000;
		overnum++; }).join();
	while (overnum != 10);

	int a = usetime;
	printf("10线程耗时: %d\n",a);
}

void BenchmarkMalloc()
{	
	std::atomic_int usetime(0);
	std::atomic_int overnum(0);
	for (int i = 0; i < 10; ++i)
		std::thread([&usetime, &overnum]()
					{
		int* p[10000]={0};
		clock_t begin = std::chrono::system_clock::now().time_since_epoch().count();
		for(int j=0;j<10;++j)
		{for(int i=0;i<10000;++i)
			p[i] = (int*)malloc(40);
		
			for(int i=0;i<10000;++i)
			free(p[i]);
		}
		clock_t end = std::chrono::system_clock::now().time_since_epoch().count();
		usetime += (end-begin)/1000/1000;
		overnum++; }).join();
	while (overnum != 10);

	int a = usetime;
	printf("10线程耗时: %d\n",a);
}

int main()
{
	std::cout << "===========================================================================" << std::endl;

	BenchmarkYMalloc();
	std::cout << "===========================================================================" << std::endl;

	BenchmarkMalloc();
	std::cout << "===========================================================================" << std::endl;
}