#pragma once

#include "lib/utility.hpp"

namespace sig
{

	/*
	//[a] -> [b] -> (a -> b -> void) -> void
	//戻り値の型がvoidの場合
	template <class R, class A, class B, template < class T, class = std::allocator<T >> class Container, typename std::enable_if<std::is_same<R, void>::value>::type*& = enabler>
	void ZipWith(Container<A> const& list1, Container<B> const& list2, std::function<void(typename std::common_type<A>::type, typename std::common_type<B>::type)> const& func)
	{
	const uint length = list1.size() < list2.size() ? list1.size() : list2.size();
	uint i = 0;
	for (auto it1 = std::begin(list1), it2 = std::begin(list2), end1 = std::end(list1), end2 = std::end(list2); i < length; ++i, ++it1, ++it2) func(*it1, *it2);
	}
	*/

	/* コンテナ */
#if SIG_ENABLE_BOOST

	//動的確保される固定長配列 (構築後のサイズ変更不可)
	template <class T, class Allocator = std::allocator<T>>
	class FixedVector
	{
	public:
	typedef T value_type;
	typedef typename boost::call_traits<T>::param_type		param_type;
	typedef typename boost::call_traits<T>::reference		reference;
	typedef typename boost::call_traits<T>::const_reference	const_reference;
	typedef typename boost::call_traits<T>::value_type		result_type;

	typedef uint size_type;

	private:
	std::vector<T> _data;

	public:
	explicit FixedVector(Allocator const& alloc = Allocator()) : _data(alloc){}
	explicit FixedVector(size_type size, param_type value = T(), Allocator const& alloc = Allocator()) : _data(size, value, alloc){}
	//	explicit FixedVector(size_type count, T value) : _data(size, value){}
	template <class InputIter> FixedVector(InputIter first, InputIter last, Allocator const& alloc = Allocator()) : _data(first, last, alloc){}
	explicit FixedVector(std::vector<T> const& src) : _data(src){}
	explicit FixedVector(std::vector<T> && src) : _data(move(src)){}

	FixedVector(FixedVector const& src) : _data(src._data){}
	FixedVector(FixedVector && src) : _data(move(src._data)){}
	FixedVector(std::initializer_list<T> init, Allocator const& alloc = Allocator()) : _data(init){}

	FixedVector& operator=(FixedVector const& src){
	*this = FixedVector(src);
	return *this;
	}
	FixedVector& operator=(FixedVector && src){
	this->_data = move(src._data);
	return *this;
	}
	FixedVector& operator=(std::initializer_list<T> ilist){
	*this = FixedVector(ilist.begin(), ilist.end());
	return *this;
	}
	FixedVector& operator=(std::vector<T> const& src){
	this->_data = src;
	return *this;
	}
	FixedVector& operator=(std::vector<T> && src){
	this->_data = move(src);
	return *this;
	}

	auto begin()->decltype(_data.begin()){ return _data.begin(); }
	auto begin() const ->decltype(_data.begin()){ return _data.begin(); }
	auto cbegin() const ->decltype(_data.cbegin()){ return _data.cbegin(); }

	auto end()->decltype(_data.end()){ return _data.end(); }
	auto end() const ->decltype(_data.end()){ return _data.end(); }
	auto cend() const ->decltype(_data.cend()){ return _data.cend(); }

	auto rbegin()->decltype(_data.rbegin()){ return _data.rbegin(); }
	auto rbegin() const ->decltype(_data.rbegin()){ return _data.rbegin(); }
	auto crbegin() const ->decltype(_data.crbegin()){ return _data.crbegin(); }

	auto rend()->decltype(_data.rend()){ return _data.rend(); }
	auto rend() const ->decltype(_data.rend()){ return _data.rend(); }
	auto crend() const ->decltype(_data.crend()){ return _data.crend(); }

	reference at(size_type pos){ return _data.at(pos); }
	const_reference at(size_type pos) const{ return _data.at(pos); }

	reference operator [](size_type pos){ return _data[pos]; }
	const_reference operator [](size_type pos) const{ return _data[pos]; }

	reference front(){ return _data.front(); }
	const_reference front() const{ return _data.front(); }

	reference back(){ return _data.back(); }
	const_reference back() const{ return _data.back(); }

	bool empty() const{ return _data.empty(); }

	size_type size() const{ return _data.size(); }

	size_type max_size() const{ return _data.max_size; }

	void swap(FixedVector& other){ _data.swap(other); }
	};
#endif

	/*
	//vector, list の積集合を求める(要素数は1個). [動作要件：T::operator==()]
	template <class T, template<class T_, class = std::allocator<T_>> class Container>
	Container<T> SetIntersection(Container<T> const& src1, Container<T> const& src2)
	{
	Container<T> result;

	for (T const& e1 : src1){
	for (T const& e2 : src2){
	if (e1 == e2 && [&result, &e2]()->bool{
	for (T const& r : result){
	if (r == e2) return false;
	}
	return true;
	}()){
	result.push_back(e2);
	}
	}
	}
	return move(result);
	}

	//unordered_set の積集合を求める.[動作要件：T::operator==()]
	template <class T>
	std::unordered_set<T> SetIntersection(std::unordered_set<T> const& src1, std::unordered_set<T> const& src2)
	{
	std::unordered_set<T> result;

	for (T const& e1 : src1){
	for (T const& e2 : src2){
	if (e1 == e2) result.insert(e2);
	}
	}
	return move(result);
	}

	//unordered_map の積集合を求める(bool key ? キーで比較【第1引数の要素を取得】 : 両方一致). [動作要件：K::operator==(), V::operator==()]
	template <class K, class V>
	std::unordered_map<K, V> SetIntersection(std::unordered_map<K, V> const& src, std::unordered_map<K, V> const& other, bool const key)
	{
	std::unordered_map<K, V> result;

	for (auto const& e : src){
	for (auto const& o : other){
	if (key && e.first == o.first) result.insert(e);
	else if (e == o) result.insert(e);
	}
	}
	return move(result);
	}


	//vector, list の差集合を求める(要素数は1個). [動作要件：T::operator==()]
	template <class T, template<class T_, class = std::allocator<T_>> class Container>
	Container<T> SetDifference(Container<T> const& src1, Container<T> const& src2)
	{
	Container<T> result, sum(src1);
	sum.insert(sum.end(), src2.begin(), src2.end());

	auto intersection = SetIntersection(src1, src2);

	for (T const& s : sum){
	if ([&intersection, &s]()->bool{
	for (T const& i : intersection){
	if (s == i) return false;
	}
	return true;
	}() && [&result, &s]()->bool{
	for (T const& r : result){
	if (s == r) return false;
	}
	return true;
	}()){
	result.push_back(s);
	}
	}
	return move(result);
	}

	//unordered_set の差集合を求める.[動作要件：T::operator==()]
	template <class T>
	std::unordered_set<T> SetDifference(std::unordered_set<T> const& src1, std::unordered_set<T> const& src2)
	{
	std::unordered_set<T> result, sum(src1);
	sum.insert(src2.begin(), src2.end());

	auto intersection = SetIntersection(src1, src2);

	for (T const& s : sum){
	if ([&intersection, &s]()->bool{
	for (T const& i : intersection){
	if (s == i) return false;
	}
	return true;
	}()){
	result.insert(s);
	}
	}
	return move(result);
	}

	//unordered_map の差集合を求める(bool key ? キーで比較 : 両方一致). [動作要件：K::operator==(), V::operator==()]
	template <class K, class V>
	std::unordered_map<K, V> SetDifference(std::unordered_map<K, V> const& src1, std::unordered_map<K, V> const& src2, bool const key)
	{
	std::unordered_map<K, V> result, sum(src1);
	sum.insert(src2.begin(), src2.end());

	auto intersection = SetIntersection(src1, src2, key);

	for (auto const& s : sum){
	if ([&intersection, &s, key]()->bool{
	for (auto const& i : intersection){
	if (key && s.first == i.first) return false;
	else if (!key && s == i) return false;
	}
	return true;
	}()){
	result.insert(s);
	}
	}
	return move(result);
	}
	*/

}