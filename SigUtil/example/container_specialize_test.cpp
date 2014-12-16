#include "container_specialize_test.h"
#include "../lib/helper/container_traits.hpp"


// ユーザ定義クラスにイテレータを返すbegin, endメンバ関数を定義し、
// sig名前空間で特殊化することでライブラリ内のコンテナを受け取る関数が使用可能になる
template <class T>
class MyVector3
{
	std::array<T, 3> vec_;
	
public:
	explicit MyVector3(T x, T y, T z) : vec_({ x, y, z }) {}

	auto begin() const ->decltype(vec_.begin()){ return vec_.begin(); }
	auto end() const ->decltype(vec_.end()){ return vec_.end(); }

	size_t size() const{ return 3; }	// これもあった方がいい
};

namespace sig
{
namespace impl{
	// コンテナの性質に合わせて、適切に特殊化するクラスを選ぶことを推奨
	template<template<class> class C, class T>
	struct static_container_traits<C<T>>
	{
		static const bool exist = true;		// 特殊化できているか

		using value_type = T;

		template<class U>
		using rebind = C<U>;

		static void add_element(C<T>& c, const T& t)
		{
			c.push_back(t);
		}
	};

	// 特殊化したクラスをcontainer_traitsに継承(登録作業)
	template<class... Args>
	struct container_traits<MyVector3<Args...>> : public static_container_traits<MyVector3<Args...>>
	{};
}
}

void ContainerSpecializeTest()
{
	MyVector3<double> vec1(1, 1.5, 2.5);
	MyVector3<int> vec2(1, -1, 1);

	auto euc = sig::euclidean_distance(vec1, vec2);
	assert(sig::equal(euc, sqrt(pow(2.5, 2)+pow(1.5, 2))));
}
