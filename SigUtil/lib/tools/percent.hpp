/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_PERCENT_HPP
#define SIG_UTIL_PERCENT_HPP

#include "../sigutil.hpp"

#if SIG_ENABLE_BOOST
#include <boost/serialization/serialization.hpp>
#endif


/*  */

namespace sig
{

// パーセント型
class Percent
{
	int percent_;

public:
	explicit Percent(int percent) : percent_(percent){}

	int get_percent() const{ return percent_; }
	double get_double() const{ return percent_ * 0.01; }

	static Percent const& unit(){ static const Percent unit(100); return unit; }

	Percent operator=(Percent src){ percent_ = src.percent_; return *this; }
	Percent operator=(int src){ percent_ = src; return *this; }

	bool operator==(Percent obj) const{ return percent_ == obj.percent_; }

	bool operator!=(Percent obj) const{ return percent_ != obj.percent_; }

#if SIG_ENABLE_BOOST
private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, unsigned int version)
	{
		ar & percent_;
	}

	template <class Archive>
	friend void save_construct_data(Archive & ar, Percent const* p, unsigned int version){};

	template <class Archive>
	friend void load_construct_data(Archive & ar, Percent* p, unsigned int version){
		::new(p) Percent(0);
	}
#endif
};

}
#endif