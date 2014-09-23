#include "string_test.h"
#include "../lib/file.hpp"
#include "../lib/tools/time_watch.hpp"

//sigutil.hpp の SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる

using TVec = std::vector<std::string>;
using TVecw = std::vector<std::wstring>;
using TVec2 = std::vector<std::vector<std::string>>;

using sig::uint;
using sig::SIG_Regex;

void RegexTest()
{
#if !((__GLIBCXX__ || __GLIBCPP__) && !SIG_ENABLE_BOOST)
	auto raw1 = "? or (lol) must be escaped";

	//エスケープ処理した文字列を取得
	auto escaped1 = sig::escape_regex(raw1);
	auto escaped2 = sig::escape_regex(L"?とか(笑)はエスケープすべき文字");

	auto test1 = R"(\? or \(lol\) must be escaped)";	assert(escaped1 == test1);
	auto test2 = LR"(\?とか\(笑\)はエスケープすべき文字)";	assert(escaped2 == test2);
	

	//エスケープ処理をしつつ std::regex(or std::wregex)を取得
	auto reg = sig::make_regex(L"(笑)");
	auto replaced = SIG_RegexReplace(std::wstring(L"てすと(笑)です"), reg, std::wstring(L""));

	assert(replaced == L"てすとです");

	//正規表現で検索
	auto matches1 = sig::regex_search("test tes1a tes2b", SIG_Regex("tes(\\d)(\\w)"));

	//auto match = *matches; //optional
	//match[0][0] == tes1a, match[0][1] == 1, match[0][2] == a
	//match[1][0] == tes2b, match[1][1] == 2, match[1][2] == b
	TVec2 test3 = { { "tes1a", "1", "a" }, { "tes2b", "2", "b" } };
	for (uint i=0; i < sig::fromJust(matches1).size(); ++i){
		for (uint j = 0; j < sig::fromJust(matches1)[i].size(); ++j){
			assert(sig::is_container_valid(matches1) && sig::fromJust(matches1)[i][j] == test3[i][j]);
		}
	}

	//正規表現で検索 (エスケープ済みの文字列を使用)
	auto matches2 = sig::regex_search("search「? or (lol) must be escaped」", SIG_Regex(escaped1));
	
	assert(sig::is_container_valid(matches2) && sig::fromJust(matches2)[0][0] == raw1);
#endif
}

void TagDealerTest()
{
	sig::TagDealer<std::string> tag_dealer("<", ">");

	auto encoded = tag_dealer.encode("test", "TAG");
	
	assert(encoded == "<TAG>test<TAG>");					

	auto decoded = tag_dealer.decode(encoded, "TAG");
	auto ignored = tag_dealer.decode(encoded, "HOO");

	assert(sig::fromJust(decoded) == "test");
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	if (ignored) assert(false);				//ignored == nothing
#else
	assert(ignored == "");
#endif
	
	//まとめてエンコード
	//encoded_vec = "<TAG1>str1<TAG1><TAG2>str2<TAG2><TAG3>str3<TAG3>"
	auto encoded_vec = tag_dealer.encode(std::list<std::string>{"str1", "str2", "str3"}, std::list<std::string>{"TAG1", "TAG2", "TAG3"});

	TVec test{ "str1", "str3" };

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	//まとめてデコード
	auto decoded_vec = tag_dealer.decode(encoded_vec, std::deque<std::string>{"TAG1", "TAG3"});

	if (decoded){
		for (uint i=0; i< decoded_vec->size(); ++i) assert((*decoded_vec)[i] == test[i]);
	}
#endif
}

void SplitTest()
{
	std::string str{ "one, 2, 参 "};

	//,をデリミタとして分割
	auto split1 = sig::split(str, ",");

	TVec test1{ "one", " 2", " 参 " };
	for (uint i = 0; i<split1.size(); ++i) assert(split1[i] == test1[i]);

	//コンテナ(シーケンス限定)の明示的指定も可
	auto split2 = sig::split<std::list>(L"https://github.com/regenschauer490/Utility", L"/");

	TVecw test2{ L"https:", L"github.com", L"regenschauer490", L"Utility" };
	auto sp2it = split2.begin();
	for (uint i = 0; i<split2.size(); ++i, ++sp2it) assert(*sp2it == test2[i]);

	//デリミタ間に何もなければ無視
	auto split3 = sig::split("10 100  1000", " ");	
	auto split4 = sig::split(" ,, ,  ,", ",");

	TVec test3{ "10", "100", "1000"};
	for (uint i = 0; i<split3.size(); ++i) assert(split3[i] == test3[i]);
	TVec test4{ " ", " ", "  " };
	for (uint i = 0; i<split4.size(); ++i) assert(split4[i] == test4[i]);

	std::string sentence = R"(1
2

4
)";

	//行毎に分割
#if SIG_CLANG_ENV
	auto split5 = sig::split(sentence, "\r\n");
#else
	auto split5 = sig::split(sentence, "\n");
#endif
	TVec test5 = { "1", "2", "4" };
	for (uint i = 0; i<split5.size(); ++i){
		assert(split5[i] == test5[i]);
	}
}

void CatStrTest()
{
	//文字列の結合
	auto cat1 = sig::cat_str(std::vector<std::string>{"eins", "zwei", "drei"}, "");
	auto cat2 = sig::cat_str(std::list<std::wstring>{L"eins", L"zwei", L"drei"}, L",");
	auto cat3 = sig::cat_str(std::set<int>{1, 2, 3}, "\n");

	assert(cat1 == "einszweidrei");
	assert(cat2 == L"eins,zwei,drei");
	assert(cat3 == "1\n2\n3");
}


void StrConvertTest()
{
#if SIG_MSVC_ENV	//windows + VisualStudio環境

	const auto sjis = sig::fromJust( sig::read_line<std::string>(L"../SigUtil/example/test_file/shift_jis.txt"));
	const auto utf8 = sig::fromJust( sig::read_line<std::string>(L"../SigUtil/example/test_file/utf8.txt"));

	//マルチ文字 <-> ワイド文字 変換
	std::wstring	wstr = sig::str_to_wstr(sjis[1]);
	std::string		str = sig::wstr_to_str(wstr);

	assert(str == sjis[1]);

	//まとめて変換
	auto wstr_vec = sig::str_to_wstr(sjis);

	//Shift-JIS <-> UTF-8
	std::string	utf8_from_sjis = sig::sjis_to_utf8(sjis[1]);
	std::string	sjis_from_utf8 = sig::utf8_to_sjis(utf8[1]);

	assert(utf8_from_sjis == utf8[1]);
	assert(sjis_from_utf8 == sjis[1]);

	//Shift-Jis <-> UTF-16
	std::u16string	utf16_from_sjis = sig::sjis_to_utf16(sjis[1]);
	std::string		sjis_from_utf16 = sig::utf16_to_sjis(utf16_from_sjis);

	assert(sjis_from_utf16 == sjis[1]);

	//UTF-8 <-> UTF-16
	std::u16string	utf16_from_utf8 = sig::utf8_to_utf16(utf8[1]);
	std::string		utf8_from_utf16 = sig::utf16_to_utf8(utf16_from_utf8);

	assert(utf8_from_utf16 == utf8[1]);

	assert(utf16_from_utf8 == utf16_from_sjis);
	
#elif SIG_GCC_ENV	//g++
	const auto utf8 = sig::fromJust( sig::read_line<std::string>("../SigUtil/example/test_file/utf8.txt"));

	//マルチ文字 <-> ワイド文字 変換
	std::wstring	wstr = sig::str_to_wstr(utf8[1]);
	std::string	str = sig::wstr_to_str(wstr);

	assert(str == utf8[1]);

	//まとめて変換
	auto wstr_vec = sig::str_to_wstr(utf8);
#else
#endif
}

void ZenHanTest()
{
	//全角 <-> 半角 変換クラス
	auto& replacer = sig::ZenHanReplace::GetInstance();

	std::wstring sentence1 = L"ＡBＣアｲウ１2３ギｶﾞﾍﾟポ";

	replacer.alphabet_zen2han(sentence1);
	assert(sentence1 == L"ABCアｲウ１2３ギｶﾞﾍﾟポ");

	replacer.katakana_zen2han(sentence1);
	assert(sentence1 == L"ABCｱｲｳ１2３ｷﾞｶﾞﾍﾟﾎﾟ");

	replacer.number_zen2han(sentence1);
	assert(sentence1 == L"ABCｱｲｳ123ｷﾞｶﾞﾍﾟﾎﾟ");


	std::wstring sentence2 = L"ＡBＣアｲウ１2３ギｶﾞﾍﾟポ";

	replacer.alphabet_han2zen(sentence2);
	assert(sentence2 == L"ＡＢＣアｲウ１2３ギｶﾞﾍﾟポ");

	replacer.katakana_han2zen(sentence2);
	assert(sentence2 == L"ＡＢＣアイウ１2３ギガペポ");

	replacer.number_han2zen(sentence2);
	assert(sentence2 == L"ＡＢＣアイウ１２３ギガペポ");
}

void StrConvertPerformanceTest()
{
	const int N = 1000;
	const auto str = "test string";

	sig::TimeWatch tw;
	for (int loop=0; loop<10; ++loop){
		sig::array<std::wstring, N> ar;

		for (int i=0; i<N; ++i){
			ar.push_back(sig::str_to_wstr(str));
		}

		tw.save();
	}

	std::cout << "str_to_wstr time: " << tw.get_total_time<std::chrono::milliseconds>() << std::endl;

	/*

	<VC++>
	[debug]
	raw	174077 / 10000
	maybe	182437 / 10000

	[release]
	raw	57379 / 10000
	maybe	114191 / 10000

	<wandbox gcc 4.10.0>
	raw	850 / 10000
	maybe	1215 / 10000

	<wandbox clang 3.5>
	raw	900 / 10000
	maybe	1489 / 10000

	*/
}
