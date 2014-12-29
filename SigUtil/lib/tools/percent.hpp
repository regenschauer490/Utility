/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIGUTIL_PERCENT_HPP
#define SIGUTIL_PERCENT_HPP

#include "../sigutil.hpp"

#if SIG_ENABLE_BOOST
#include <boost/serialization/serialization.hpp>
#endif


namespace sig
{

/// パーセント型
/**
	Percent pc1(100);
	Percent pc2(50);
	Percent pc3(0);
	Percent pc4(0);

	pc3 = 1;					// copy assignment. pc4 == 1%
	pc4 = Percent::unit();		// copy assignment. pc3 == 100%

	assert(pc1.get_percent() == 100 && equal(pc1.get_double(), 1));		// 100% = 1
	assert(pc2.get_percent() == 50 && equal(pc2.get_double(), 0.5));	// 50% = 0.5
	assert(pc3.get_percent() == 1 && equal(pc3.get_double(), 0.01));	// 1% = 0.01
	assert(pc1 == pc4);
*/
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