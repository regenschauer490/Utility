#include "string_test.h"
#include "../lib/file.hpp"

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる

using TVec = std::vector<std::string>;
using TVecw = std::vector<std::wstring>;
using TVec2 = std::vector<std::vector<std::string>>;

using sig::uint;
using sig::SIG_Regex;

void RegexTest()
{
#if SIG_MSVC_ENV
	//エスケープ処理した文字列を取得
	auto escaped1 = sig::RegexEscaper("? or (lol) must be escaped");
	auto escaped2 = sig::RegexEscaper(L"?とか(笑)はエスケープすべき文字");

	auto test1 = R"(\? or \(lol\) must be escaped)";	assert(escaped1 == test1);
	auto test2 = LR"(\?とか\(笑\)はエスケープすべき文字)";	assert(escaped2 == test2);
	

	//エスケープ処理をしつつ std::regex(or std::wregex)を取得
	auto reg = sig::RegexMaker(L"(笑)");
	auto replaced = SIG_RegexReplace(std::wstring(L"てすと(笑)です"), reg, std::wstring(L""));

	assert(replaced == L"てすとです");
#else
	auto escaped1 = R"(? or (lol) must be escaped)";
#endif

	//正規表現で検索
	auto matches1 = sig::RegexSearch("test tes1a tes2b", SIG_Regex("tes(\\d)(\\w)"));
	auto matches2 = sig::RegexSearch("search「? or (lol) must be escaped」", SIG_Regex(escaped1));
	
	TVec2 test3 = { { "tes1a", "1", "a" }, { "tes2b", "2", "b" } };
	auto test4 = "? or (lol) must be escaped";

	for (uint i=0; i < sig::FromJust(matches1).size(); ++i){
		for (uint j = 0; j < sig::FromJust(matches1)[i].size(); ++j){
			assert(sig::FromJust(matches1)[i][j] == test3[i][j]);
		}
	}
	for (auto m : sig::FromJust(matches2)){
		assert( m.front() == test4);
	}

}

void TagDealerTest()
{
	sig::TagDealer<std::string> tag_dealer("<", ">");

	auto encoded = tag_dealer.Encode("test", "TAG");
	
	assert(encoded == "<TAG>test<TAG>");					

	auto decoded = tag_dealer.Decode(encoded, "TAG");
	auto ignored = tag_dealer.Decode(encoded, "HOO");

	assert(sig::FromJust(decoded) == "test");
#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	if (ignored) assert(false);				//ignored == nothing
#else
	assert(ignored == "");
#endif
	
	//まとめてエンコード
	//encoded_vec = "<TAG1>str1<TAG1><TAG2>str2<TAG2><TAG3>str3<TAG3>"
	auto encoded_vec = tag_dealer.Encode(std::list<std::string>{"str1", "str2", "str3"}, std::list<std::string>{"TAG1", "TAG2", "TAG3"});

	TVec test{ "str1", "str3" };

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	//まとめてデコード
	auto decoded_vec = tag_dealer.Decode(encoded_vec, std::deque<std::string>{"TAG1", "TAG3"});

	if (decoded){
		for (uint i=0; i< decoded_vec->size(); ++i) assert((*decoded_vec)[i] == test[i]);
	}
#endif
}

void SplitTest()
{
	std::string str{ "one, 2, 参 "};
	//,をデリミタとして分割
	auto split1 = sig::Split(str, ",");

	TVec test1{ "one", " 2", " 参 " };
	for (uint i = 0; i<split1.size(); ++i) assert(split1[i] == test1[i]);

	//コンテナ(シーケンス限定)の明示的指定も可
	auto split2 = sig::Split<std::list>(L"https://github.com/regenschauer490/Utility", L"/");

	TVecw test2{ L"https:", L"github.com", L"regenschauer490", L"Utility" };
	auto sp2it = split2.begin();
	for (uint i = 0; i<split2.size(); ++i, ++sp2it) assert(*sp2it == test2[i]);

	//デリミタ間に何もなければ無視
	auto split3 = sig::Split("10 100  1000", " ");	
	auto split4 = sig::Split(" ,, ,  ,", ",");

	TVec test3{ "10", "100", "1000"};
	for (uint i = 0; i<split3.size(); ++i) assert(split3[i] == test3[i]);
	TVec test4{ " ", " ", "  " };
	for (uint i = 0; i<split4.size(); ++i) assert(split4[i] == test4[i]);

	auto sentence = R"(1
2

4
)";
	//行毎に分割（空行無視）
	auto split5 = sig::Split(sentence, "\n");
	TVec test5 = { "1", "2", "4" };
	for (uint i = 0; i<split5.size(); ++i) assert(split5[i] == test5[i]);
}

void CatStrTest()
{
	//文字列の結合
	auto cat1 = sig::CatStr(std::vector<std::string>{"eins", "zwei", "drei"}, "");
	auto cat2 = sig::CatStr(std::list<std::wstring>{L"eins", L"zwei", L"drei"}, L",");
	auto cat3 = sig::CatStr(std::set<int>{1, 2, 3}, "\n");

	assert(cat1 == "einszweidrei");
	assert(cat2 == L"eins,zwei,drei");
	assert(cat3 == "1\n2\n3");
}

void StrConvertTest()
{
	const auto sjis = sig::FromJust( sig::ReadLine<std::string>(L"../SigUtil/example/test_file/shift_jis.txt"));
	const auto utf8 = sig::FromJust( sig::ReadLine<std::string>(L"../SigUtil/example/test_file/utf8.txt"));

	//マルチ文字 <-> ワイド文字 変換
	std::wstring	wstr = sig::STRtoWSTR(sjis[1]);
	std::string		str = sig::WSTRtoSTR(wstr);

	assert(str == sjis[1]);

	//まとめて変換
	auto wstr_vec = sig::STRtoWSTR(sjis);

#if SIG_MSVC_ENV	//windows + VisualStudio環境

	//Shift-JIS <-> UTF-8
	std::string	utf8_from_sjis = sig::SJIStoUTF8(sjis[1]);
	std::string	sjis_from_utf8 = sig::UTF8toSJIS(utf8[1]);

	assert(utf8_from_sjis == utf8[1]);
	assert(sjis_from_utf8 == sjis[1]);

	//Shift-Jis <-> UTF-16
	std::u16string	utf16_from_sjis = sig::SJIStoUTF16(sjis[1]);
	std::string		sjis_from_utf16 = sig::UTF16toSJIS(utf16_from_sjis);

	assert(sjis_from_utf16 == sjis[1]);

	//UTF-8 <-> UTF-16
	std::u16string	utf16_from_utf8 = sig::UTF8toUTF16(utf8[1]);
	std::string		utf8_from_utf16 = sig::UTF16toUTF8(utf16_from_utf8);

	assert(utf8_from_utf16 == utf8[1]);

	assert(utf16_from_utf8 == utf16_from_sjis);
	
#else
#endif
}

void ZenHanTest()
{
	//全角 <-> 半角 変換クラス
	auto& replacer = sig::ZenHanReplace::GetInstance();

	std::wstring sentence1 = L"ＡBＣアｲウ１2３ギｶﾞﾍﾟポ";

	replacer.Alphabet_Zen2Han(sentence1);
	assert(sentence1 == L"ABCアｲウ１2３ギｶﾞﾍﾟポ");

	replacer.Katakana_Zen2Han(sentence1);
	assert(sentence1 == L"ABCｱｲｳ１2３ｷﾞｶﾞﾍﾟﾎﾟ");

	replacer.Number_Zen2Han(sentence1);
	assert(sentence1 == L"ABCｱｲｳ123ｷﾞｶﾞﾍﾟﾎﾟ");


	std::wstring sentence2 = L"ＡBＣアｲウ１2３ギｶﾞﾍﾟポ";

	replacer.Alphabet_Han2Zen(sentence2);
	assert(sentence2 == L"ＡＢＣアｲウ１2３ギｶﾞﾍﾟポ");

	replacer.Katakana_Han2Zen(sentence2);
	assert(sentence2 == L"ＡＢＣアイウ１2３ギガペポ");

	replacer.Number_Han2Zen(sentence2);
	assert(sentence2 == L"ＡＢＣアイウ１２３ギガペポ");
}
