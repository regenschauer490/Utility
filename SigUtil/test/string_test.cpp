#include "string_test.hpp"

void TagDealerTest()
{
	sig::TagDealer<std::string> tag_dealer("<", ">");

	auto encoded = tag_dealer.Encode("test", "TAG");
	std::cout << encoded << std::endl;

	auto decoded = tag_dealer.Decode(encoded, "TAG");
	auto ignored = tag_dealer.Decode(encoded, "HOO");

#if ENABLE_BOOST
	if (decoded) std::cout << *decoded << std::endl;
	if (ignored) std::cout << *ignored << std::endl;
#else
	std::cout << decoded << std::endl;
	std::cout << ignored << std::endl;
#endif
}

