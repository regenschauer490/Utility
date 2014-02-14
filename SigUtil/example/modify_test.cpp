#include "modify_test.h"
#include "../lib/functional.hpp"

template <class T>
using TVec = std::vector<T>;

void SortTest()
{

}

void RemoveDuplicateTest()
{
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::multiset<int, std::greater<int>> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data4{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	auto removed1 = sig::RemoveDuplicates(data1, true);
	auto removed2 = sig::RemoveDuplicates(data2, true);
	auto removed3 = sig::RemoveDuplicates(data3, true);
	auto removed4 = sig::RemoveDuplicates(data4, true);
	
	
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data1, TVec<int>{ 1, 5, 3, 0, 4 });
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, removed1, TVec<int>{ 3, 0, 1, 3 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data2, TVec<int>{ 1, 5, 3, 0, 4 });
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, removed2, TVec<int>{ 3, 0, 1, 3 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data3, TVec<int>{ 5, 4, 3, 1, 0 });
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, removed3, TVec<int>{ 3, 3, 1, 0 });

	
	auto testd4 = { 1, 5, 3, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testd4.begin(), testd4.end(), 0.0));

	auto testr4 = {3, 0, 1, 3};
	assert(std::accumulate(removed4.begin(), removed4.end(), 0.0) == std::accumulate(testr4.begin(), testr4.end(), 0.0));

#if _MSC_VER > 1800
	std::multimap<int, std::string> data5{ { 1, "a"}, {5, "b"}, {3, "c"}, {3, "d"}, {0, "e"}, {4, "f"}, {0, "g"}, {1, "h"}, {3, "c"} };
	auto removed5 = sig::RemoveDuplicates(data5, true);

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data5, std::multimap<int, std::string>{ { 1, "a"}, {5, "b"}, {3, "c"}, {3, "d"}, {0, "e"}, {4, "f"}, {0, "g"}, {1, "h"} };);
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, removed5, std::multimap<int, std::string>{{3, "c"}});
#endif
}

void RemoveTest()
{
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::multiset<int, std::greater<int>> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data4{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	//íœ—v‘f‚ª‚ ‚ê‚Î1, –³‚¯‚ê‚Î0‚ğ•Ô‚·
	assert( sig::RemoveOne(data1, 3) );
	assert( sig::RemoveOneIf(data2, [](int v){ return v == 3; }) );
	assert( sig::RemoveOne(data3, 3) );
	assert( sig::RemoveOne(data4, 3) );

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data1, TVec<int>{ 1, 5, 3, 0, 4, 0, 1, 3 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data2, TVec<int>{ 1, 5, 3, 0, 4, 0, 1, 3 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data3, TVec<int>{ 5, 4, 3, 3, 1, 1, 0, 0 });

	auto testd4 = { 1, 1, 5, 3, 3, 0, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testd4.begin(), testd4.end(), 0.0));


	//íœ—v‘f‚ª‚ ‚ê‚Î1, –³‚¯‚ê‚Î0‚ğ•Ô‚·
	assert( sig::RemoveAll(data1, 3) );
	assert( sig::RemoveAllIf(data2, [](int v){ return v == 3; }) );
	assert( sig::RemoveAll(data3, 3) );
	assert( sig::RemoveAll(data4, 3) );

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data1, TVec<int>{ 1, 5, 0, 4, 0, 1 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data2, TVec<int>{ 1, 5, 0, 4, 0, 1 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data3, TVec<int>{ 5, 4, 1, 1, 0, 0 });

	auto testda4 = { 1, 1, 5, 0, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testda4.begin(), testda4.end(), 0.0));
}