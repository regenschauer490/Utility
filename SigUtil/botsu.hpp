#pragma once

#include "utility.hpp"

namespace sig
{
	//[a] -> (a -> r) -> [r]
	//戻り値の型Rは明示的に指定が必要
	template <class OutputContainer, class InputIter, class Functor>
	OutputContainer Map(InputIter begin, InputIter end, std::size_t size, Functor func)
	{
		OutputContainer result;
		if (result.empty()) result.resize(size);
		std::transform(begin, end, std::begin(result), func);
		return std::move(result);
	}
	template < class R, class A, template < class T, class = std::less<T>, class = std::allocator<T >> class Container>
	Container<R> Map(Container<A> const& set, std::function<typename std::common_type<R>::type(typename std::common_type<A>::type)> const& func)
	{
		Container<R> result;
		std::transform(set.begin(), set.end(), std::inserter(result, result.begin()), func);
		return std::move(result);
	}

	/* コンテナ */
#if ENABLE_BOOST

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


}