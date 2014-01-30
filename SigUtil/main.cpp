#include "lib/string.hpp"
#include "lib/file.hpp"

#include "test/string_test.hpp"
#include "test/file_test.hpp"

/*
#include "utility.hpp"

#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#if SIG_ENABLE_BOOST
#include <boost/array.hpp>
#endif

struct Test1
{
	int a_;
	double b_;
};

void MapTest()
{
	//dynamically allocated container (ordered list-type)
	std::vector<int> v{ 1, 2, 3, 4, 5 };
	std::list<double> l{ 1.1, 2.2, 3.3 };
	std::deque<int> dq{ 1, 2, 3 };
	//auto m = sig::Map<std::multiset<double>>(v, [](int v){ return (v % 2) * 1.5; });
	auto m1 = sig::Map<int>(v, [](int v){ return 2 * v; });
	auto m2 = sig::Map<double>(l, [](double v){ return 2 * v; });
	auto m3 = sig::Map<double>(dq, [](int v){ return 1.5 * v; });
	
	//statically allocated container
	std::array<int, 5> ar{{1, 2, 3, 4, 5}};
	bool rar [] = { false, true, false };
	auto m4 = sig::Map<int>(ar, [](int v){ return 2*v; });
	auto m5 = sig::Map<bool>(rar, [](bool v){ return !v; });

#if SIG_ENABLE_BOOST
	boost::array<int, 5> bar{ { 1, 2, 3, 4, 5 } };
	auto m6 = sig::Map<bool>(bar, [](int v){ return v < 3; });
#endif
}

*/

int main()
{
	setlocale(LC_ALL, "Japanese");

	RegexTest();
	//TagDealerTest();
	//SplitTest();
	//CatStrTest();
	//StrConvertTest();

	//GetDirectoryNamesTest();
	//FileSaveLoadTest();

	return 0;
}