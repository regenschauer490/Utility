/*
Copyright(c) 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_RATING_MATRIX_H
#define SIGUTIL_RATING_MATRIX_H

#include "../modify/remove.hpp"

#if SIG_ENABLE_BOOST
#include <boost/range/iterator_range.hpp>
#include <boost/range/join.hpp>
#endif

namespace sig
{

using Id = uint;
using userId = uint;
using ItemId = uint;

template<class T> using VectorU = std::vector<T>;		// user
template<class T> using VectorI = std::vector<T>;		// item

namespace impl
{
template <class T>
struct Rating_ : boost::noncopyable
{
	T value_;
	uint user_id_;
	uint item_id_;
	bool is_duplicate_;

public:
	Rating_(T value, uint user_id, uint item_id)
	: value_(value), user_id_(user_id), item_id_(item_id), is_duplicate_(false){}
};
}	//impl

template <class T>
using RatingPtr = std::shared_ptr<impl::Rating_<T>>;
template <class T>
using C_RatingPtr = std::shared_ptr<const impl::Rating_<T>>;

template <class T>
using RatingVector = std::vector<RatingPtr<T>>;

namespace impl
{
// user-item ratings for sparse matrix
template <class T>
class SparseRatingMatrixBase_ : boost::noncopyable
{
	using UserRatings = VectorU<RatingVector<T>>;
	using ItemRatings = VectorI<RatingVector<T>>;
	
public:
	using iterator = typename RatingVector<T>::iterator;
	using const_iterator = typename RatingVector<T>::const_iterator;
	using iterator_range = decltype(boost::make_iterator_range(std::declval<iterator>(), std::declval<iterator>()));
	using const_iterator_range = decltype(boost::make_iterator_range(std::declval<const_iterator>(), std::declval<const_iterator>()));
	using rating_range = std::vector<iterator_range>;
	using const_rating_range = std::vector<const_iterator_range>;
	//using const_joined_range = boost::range::joined_range<iterator_range, iterator_range>;
	
protected:
	UserRatings user_;
	ItemRatings item_;

protected:
	SparseRatingMatrixBase_() = default;
	SparseRatingMatrixBase_(UserRatings u_src, ItemRatings i_src) : user_(std::move(u_src)), item_(std::move(i_src)){}

	virtual ~SparseRatingMatrixBase_() = default;

	void remove_duplicate() {
		for (auto& u : user_) sig::remove_duplicates(u);
		for (auto& i : item_) sig::remove_duplicates(i);
	}
	
public:
	// return pair(begin, end)
	auto getUsers() const->const_rating_range{
		return sig::map([](RatingVector<T> const& u){ return boost::make_iterator_range(std::begin(u), std::end(u)); }, user_);
	}

	auto getItems() const->const_rating_range{
		return sig::map([](RatingVector<T> const& i){ return boost::make_iterator_range(std::begin(i), std::end(i)); }, item_);
	}
	
	auto getValue(UserId u_id, ItemId i_id) const->sig::Maybe<T>{
		for (auto&& e : user_[u_id]){
			if(e->item_id_ == i_id) return sig::Just(e->value_);
		}
		return sig::Nothing<T>();
	}

	bool userEmpty(UserId id) const{ return id < user_.size() ? user_[id].empty() : false; }
	bool itemEmpty(ItemId id) const{ return id < item_.size() ? item_[id].empty() : false; }
	
	uint userSize() const{
		return user_.size();
	}
	uint itemSize() const{
		return item_.size(); 
	}
};

// boolean user-item ratings(exist or not) for sparse matrix
class SparseBooleanMatrix_ : public SparseRatingMatrixBase_<int>
{
	using T = int;

	template <class Range>
	void reconstruct(Range const& ratings)
	{
		uint umax = 0, imax = 0;
		for(auto const& e : ratings){
			if(e->user_id_ > umax) umax = e->user_id_;
			if(e->item_id_ > imax) imax = e->item_id_;
		}
		
		if(user_.size() <= umax) user_.resize(umax+1);
		if(item_.size() <= imax) item_.resize(imax+1);
		
		for(auto const& e : ratings){
			user_[e->user_id_].push_back(e);
			item_[e->item_id_].push_back(e);
		}
	}

public:
	SparseBooleanMatrix_(std::vector<std::vector<Id>> const& ratings, bool is_user_rating)
	{
		std::unordered_map<Id, std::vector<RatingPtr<T>>> id_rating_map;
		uint max_col = 0;

		if (is_user_rating) user_.resize(ratings.size());
		else item_.resize(ratings.size());

		for (uint row_id = 0; row_id < ratings.size(); ++row_id){
			for (Id col_id : ratings[row_id]){
				if (is_user_rating){
					auto rating = std::make_shared<Rating_<T>>(1, row_id, col_id);
					user_[row_id].push_back(rating);
					id_rating_map[col_id].push_back(rating);
				}
				else{
					auto rating = std::make_shared<Rating_<T>>(1, col_id, row_id);
					item_[row_id].push_back(rating);
					id_rating_map[col_id].push_back(rating);
				}
				
				if(col_id > max_col ) max_col = col_id;
			}
		}

		if (is_user_rating) item_.resize(max_col + 1);
		else user_.resize(max_col + 1);

		for (Id id = 0; id <= max_col; ++id){
			if (is_user_rating){
				item_[id] = std::move(id_rating_map[id]);
			}
			else{
				user_[id] = std::move(id_rating_map[id]);
			}
		}
	}
	
	SparseBooleanMatrix_(std::vector<RatingVector<T>> const& ratings){
		for (auto&& r : ratings) reconstruct(boost::make_iterator_range(std::begin(r), std::end(r)));
		remove_duplicate();
	}

	SparseBooleanMatrix_(const_iterator_range const& ratings){
		reconstruct(ratings);
		remove_duplicate();
	}
};

}	// impl


template<class T>
using SparseRatingMatrix = impl::SparseRatingMatrixBase_<T>;

template <class T>
using RatingMatrixPtr = std::shared_ptr<SparseRatingMatrix<T>>;

using SparseBooleanMatrix = impl::SparseBooleanMatrix_;

using BooleanMatrixPtr = std::shared_ptr<SparseBooleanMatrix>;

}
#endif