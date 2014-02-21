#include "modify_test.h"
#include "../lib/functional.hpp"

template <class T>
using TVec = std::vector<T>;

void SortTest()
{
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	//ソート前のindexを保持してソート
	//ex: [30, 50, -10, 0] -> ([-10, 0, 30, 50], [2, 3, 0, 1])
	auto swi1 = sig::SortWithIndex(data1);	//container<T> -> tuple<container<T>, vector<uint>>
	auto sorted1 = std::get<0>(swi1);
	auto original_index1 = std::get<1>(swi1);

	auto test1 = std::make_tuple(TVec<int>{ 0, 0, 1, 1, 3, 3, 3, 4, 5}, TVec<sig::uint>{ 4, 6, 0, 7, 2, 3, 8, 5, 1 });
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, sorted1, std::get<0>(test1));
	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, original_index1, std::get<1>(test1));


	auto swi2 = sig::SortWithIndex(data2, [](int l, int r){ return l > r; });
	auto sorted2 = std::get<0>(swi2);

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, sorted2, sig::Reverse(std::get<0>(test1)));
}

void ShuffleTest()
{
	std::deque<int> data0{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	sig::Shuffle(data0);

	//複数のコンテナを対応付けつつシャッフル
	//ex: c1[1, 2, 3, 4], c2[1, 2, 3, 4] -> c1'[3, 1, 4, 2], c2'[3, 1, 4, 2]
	sig::Shuffle(data1, data2);

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data1, data2);
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

	//削除要素があれば1, 無ければ0を返す
	assert( sig::RemoveOne(data1, 3) );
	assert( sig::RemoveOneIf(data2, [](int v){ return v == 3; }) );
	assert( sig::RemoveOne(data3, 3) );
	assert( sig::RemoveOne(data4, 3) );

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data1, TVec<int>{ 1, 5, 3, 0, 4, 0, 1, 3 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data2, TVec<int>{ 1, 5, 3, 0, 4, 0, 1, 3 });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, data3, TVec<int>{ 5, 4, 3, 3, 1, 1, 0, 0 });

	auto testd4 = { 1, 1, 5, 3, 3, 0, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testd4.begin(), testd4.end(), 0.0));


	//削除要素があれば1, 無ければ0を返す
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