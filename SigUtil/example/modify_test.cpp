#include "modify_test.h"
#include "../lib/functional.hpp"
#include "debug.hpp"

using namespace sig;

template <class T>
using TVec = std::vector<T>;

void SortTest()
{
	array<double, 3> data1{ 3.3, 1.1, -2.2 };
	std::vector<int> data2{ 1, 5, 3, -2 };
	std::list<int> data3{ 1, 5, 3, -2 };

	const std::vector<int> cdata1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	const std::list<int> cdata2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	array<TestInt, 9> data4{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };


	sort(data1, std::less<double>());
	assert_foreach(Identity(), data1, TVec<double>{ -2.2, 1.1, 3.3 });

	sort(data2, std::less<int>());
	assert_foreach(Identity(), data2, TVec<int>{ -2, 1, 3, 5 });

	sort(data3, std::greater<int>());
	assert_foreach(Identity(), data3, TVec<int>{ 5, 3, 1, -2 });


	//ソート前のindexを保持してソート
	const auto swi1 = sort_with_index(cdata1, std::less<int>());	//container<T> -> tuple<container<T>, vector<uint>>

	const auto sorted1 = std::get<0>(swi1);
	const auto original_index1 = std::get<1>(swi1);
	const auto test1 = std::make_tuple(TVec<int>{ 0, 0, 1, 1, 3, 3, 3, 4, 5}, TVec<uint>{ 4, 6, 0, 7, 2, 3, 8, 5, 1 });

	assert_foreach(Identity(), sorted1, std::get<0>(test1));
	assert_foreach(Identity(), original_index1, std::get<1>(test1));

	//ソート基準を指定する場合
	const auto swi2 = sort_with_index(cdata2, [](int l, int r){ return l > r; });
	const auto sorted2 = std::get<0>(swi2);

	assert_foreach(Identity(), sorted2, reverse(std::get<0>(test1)));

	// move
	const auto swi4 = sort_with_index(std::move(data4), [](TestInt const& l, TestInt const& r){ return l.value() < r.value(); });
	assert(data4[0].empty() && data4[8].empty());
}

void ShuffleTest()
{
	std::deque<int> data0{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	//普通のシャッフル
	shuffle(data0);

	//複数のコンテナを対応付けつつシャッフル
	//ex: c1[1, 2, 3, 4], c2[1, 2, 3, 4] -> c1'[3, 1, 4, 2], c2'[3, 1, 4, 2]
	shuffle(data1, data2);

	assert_foreach(Identity(), data1, data2);
}

void RemoveDuplicateTest()
{
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::multiset<int, std::greater<int>> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data4{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	std::map<int, uint> removed1 = remove_duplicates(data1);
	auto removed2 = remove_duplicates(data2);
	auto removed3 = remove_duplicates(data3);
	auto removed4 = remove_duplicates(data4);

	auto test_remove_duplicates = [](std::map<int, uint>& removed){
		assert(removed.size() == 5);
		assert(removed[0] == 1);
		assert(removed[1] == 1);
		assert(removed[3] == 2);
		assert(removed[4] == 0);
		assert(removed[5] == 0);
	};

	assert_foreach(Identity(), data1, TVec<int>{ 1, 5, 3, 0, 4 });
	test_remove_duplicates(removed1);

	assert_foreach(Identity(), data2, TVec<int>{ 1, 5, 3, 0, 4 });
	test_remove_duplicates(removed2);

	assert_foreach(Identity(), data3, TVec<int>{ 5, 4, 3, 1, 0 });
	test_remove_duplicates(removed3);

	auto testd4 = { 1, 5, 3, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testd4.begin(), testd4.end(), 0.0));
	test_remove_duplicates(removed4);

#if SIG_GCC_ENV || SIG_CLANG_ENV || _MSC_VER >= 1900
	std::multimap<int, std::string> data5{ { 1, "a"}, {5, "b"}, {3, "c"}, {3, "d"}, {0, "e"}, {4, "f"}, {0, "g"}, {1, "h"}, {3, "c"} };
	auto removed5 = remove_duplicates(data5);

	std::multimap<int, std::string> testd5{ { 1, "a"}, {5, "b"}, {3, "c"}, {3, "d"}, {0, "e"}, {4, "f"}, {0, "g"}, {1, "h"} };
	assert_foreach(Identity(), data5, testd5);
	assert(removed5.size() == 8);
	assert(removed5[std::make_pair(1, "a")] == 0);
	assert(removed5[std::make_pair(5, "b")] == 0);
	assert(removed5[std::make_pair(3, "c")] == 1);
	assert(removed5[std::make_pair(3, "d")] == 0);
	assert(removed5[std::make_pair(0, "e")] == 0);
	assert(removed5[std::make_pair(4, "f")] == 0);
	assert(removed5[std::make_pair(0, "g")] == 0);
	assert(removed5[std::make_pair(1, "h")] == 0);
#endif
}

void RemoveTest()
{
	array<std::string, 4> data0{ "a", "b", "c", "b" };
	std::vector<int> data1{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::list<int> data2{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::multiset<int, std::greater<int>> data3{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };
	std::unordered_multiset<int> data4{ 1, 5, 3, 3, 0, 4, 0, 1, 3 };

	//削除要素があれば1, 無ければ0を返す
	assert( remove_one(data0, "b") );
	assert( remove_one(data1, 3) );
	assert( remove_one_if(data2, [](int v){ return v == 3; }) );
	assert( remove_one(data3, 3) );
	assert( remove_one(data4, 3) );

	assert_foreach(Identity(), data0, TVec<std::string>{ "a", "c", "b" });
	assert_foreach(Identity(), data1, TVec<int>{ 1, 5, 3, 0, 4, 0, 1, 3 });
	assert_foreach(Identity(), data2, TVec<int>{ 1, 5, 3, 0, 4, 0, 1, 3 });
	assert_foreach(Identity(), data3, TVec<int>{ 5, 4, 3, 3, 1, 1, 0, 0 });

	auto testd4 = { 1, 1, 5, 3, 3, 0, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testd4.begin(), testd4.end(), 0.0));


	//削除要素があれば1, 無ければ0を返す
	assert( remove_all(data1, 3) );
	assert( remove_all_if(data2, [](int v){ return v == 3; }) );
	assert( remove_all(data3, 3) );
	assert( remove_all(data4, 3) );

	assert_foreach(Identity(), data1, TVec<int>{ 1, 5, 0, 4, 0, 1 });

	assert_foreach(Identity(), data2, TVec<int>{ 1, 5, 0, 4, 0, 1 });

	assert_foreach(Identity(), data3, TVec<int>{ 5, 4, 1, 1, 0, 0 });

	auto testda4 = { 1, 1, 5, 0, 0, 4 };
	assert(std::accumulate(data4.begin(), data4.end(), 0.0) == std::accumulate(testda4.begin(), testda4.end(), 0.0));
}
