#ifndef __SIG_UTIL_STRING__
#define __SIG_UTIL_STRING__

#include "sigutil.hpp"
#include "functional.hpp"
#include <regex>
#include <stdlib.h>
#include <sstream>

/* 文字列処理関連 */

namespace sig{

	template <class T>
	struct StringId{};
	template <>
	struct StringId<std::string>{ typedef std::string type; };
	template <>
	struct StringId<char const*>{ typedef std::string type; };
	template <>
	struct StringId<std::wstring>{ typedef std::wstring type; };
	template <>
	struct StringId<wchar_t const*>{ typedef std::wstring type; };

	template <class T>
	using TString = typename StringId<typename std::decay<T>::type>::type;

	template <class T>
	struct Str2RegexSelector{ typedef void type; };
	template <>
	struct Str2RegexSelector<std::string>{ typedef std::regex type; };
	template <>
	struct Str2RegexSelector<std::wstring>{ typedef std::wregex type; };

	template <class T>
	struct Str2SmatchSelector{ typedef void type; };
	template <>
	struct Str2SmatchSelector<std::string>{ typedef std::smatch type; };
	template <>
	struct Str2SmatchSelector<std::wstring>{ typedef std::wsmatch type; };


	//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープする
	inline auto RegexEscaper(std::string const& expression) ->std::string
	{
		static const std::regex escape_reg("([(){}\\[\\]|^?$.+*\\\\])");
		return std::regex_replace(expression, escape_reg, "\\$1");
	}
	inline auto RegexEscaper(std::wstring const& expression) ->std::wstring
	{
		static const std::wregex escape_reg(L"([(){}\\[\\]|^?$.+*\\\\])");
		return std::regex_replace(expression, escape_reg, L"\\$1");
	}

	//自動でエスケープしてstd::regex or std::wregex を返す
	template <class T>
	auto RegexMaker(T const& expression) ->typename Str2RegexSelector<TString<T>>::type
	{
		return typename Str2RegexSelector<TString<T>>::type(RegexEscaper(expression));
	}


	//std::regex_search のラッパ関数。
	//return -> maybe ? [マッチした箇所の順番][マッチ内の参照の順番. 0は全文, 1以降は参照箇所] : nothing
	//例：
	//src = "test tes1 tes2"
	//expression = std::regex("tes(\\d)")
	//return -> [[tes1, 1], [tes2, 2]]
	template <template <class T_, class = std::allocator<T_>> class Container = std::vector, class T = std::string >
	auto RegexSearch(T src, typename Str2RegexSelector<TString<T>>::type const& expression) ->typename Just< Container< Container<TString<T>>>>::type
	{
		Container<Container<TString<T>>> d;
		typename Str2SmatchSelector<TString<T>>::type match;
		auto tmp = TString<T>(src);

		while (std::regex_search(tmp, match, expression)){
			d.push_back(Container<TString<T>>());
			for (auto const& m : match) d.back().push_back(m);
			tmp = match.suffix().str();
		}

		return d.empty() ? Nothing(std::move(d)) : typename Just<Container<Container<TString<T>>>>::type(std::move(d));
	}

/*
	//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープしてから処理（推奨）
	template < class T, template < class T_, class = std::allocator<T_>> class Container = std::vector >
	auto RegexSearch(T src, T expression) ->typename Just< Container< Container<String<T>>>>::type
	{
		return RegexSearch(src, RegexMaker(expression));
	}
*/

	//HTML風にタグをエンコード・デコードする
	//例：　<TAG>text<TAG>
	template <class String>
	class TagDealer
	{
		const String tel_;
		const String ter_;

	public:
		TagDealer(String tag_encloser_left, String tag_encloser_right) : tel_(tag_encloser_left), ter_(tag_encloser_right){};

		String Encode(String const& src, String const& tag) const{
			auto tag_str = tel_ + tag + ter_;
			return tag_str + src + tag_str;
		}
		
		auto Decode(String const& src, String const& tag) ->typename Just<String>::type const{
			auto tag_str = tel_ + tag + ter_;
			auto parse = Split(" " + src, tag_str);
			return parse.size() < 2 ? Nothing(String()) : typename Just<String>::type(parse[1]);
		}

		template < template < class T_, class Allocator = std::allocator<T_>> class Container >
		String Encode(Container<String> const& src, Container<String> const& tag) const;

#if SIG_ENABLE_BOOST
		template < template < class T_, class Allocator = std::allocator<T_>> class Container >
		auto Decode(String const& src, Container<String> const& tag) ->typename Just<Container<String>>::type const;
#endif
	};

	template <class String>
	template < template < class T_, class Allocator = std::allocator<T_>> class Container >
	String TagDealer<String>::Encode(Container<String> const& src, Container<String> const& tag) const
	{
		auto size = std::min(src.size(), tag.size());
		auto calc = ZipWith([&](Container<String>::value_type s, Container<String>::value_type t){ return Encode(s, t); }, src, tag);
		return std::accumulate(calc.begin(), calc.end(), String(""));
	}

#if SIG_ENABLE_BOOST
	template <class String>
	template < template < class T_, class Allocator = std::allocator<T_>> class Container >
	auto TagDealer<String>::Decode(String const& src, Container<String> const& tag) ->typename Just<Container<String>>::type const
	{
		Container<String> result;
		for (auto const& e : tag){
			if (auto d = Decode(src, e)) result.push_back(*d);
		}
		return result.empty() ? Nothing(String()) : typename Just<Container<String>>::type(std::move(result));
	}
#endif

	//文字列(src)をある文字列(delim)を目印に分割する
	//戻り値のコンテナはデフォルトではvector
	//例：src="one, 2, 参 ", delim="," -> return vector<string>{"one", " 2", " 参 "}
	template <template <class T_, class = std::allocator<T_>> class Container = std::vector,  class String = std::string >
	auto Split(String src, typename std::common_type<String>::type const& delimiter) ->Container<typename StringId<String>::type>
	{
		Container<String> result;
		int const mag = delimiter.size();
		int cut_at;

		if (!mag) return Container<String>{src};

		while ((cut_at = src.find(delimiter)) != src.npos){
			if (cut_at > 0) result.push_back(src.substr(0, cut_at));
			src = src.substr(cut_at + mag);
		}
		if (src.length() > 0){
			result.push_back(src);
		}

		return std::move(result);
	}

	template < template <class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<std::string> Split(char const* const src, char const* const delimiter)
	{
		return Split<Container>(std::string(src), std::string(delimiter));
	}

	template < template < class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<std::wstring> Split(wchar_t const* const src, wchar_t const* const delimiter)
	{
		return Split<Container>(std::wstring(src), std::wstring(delimiter));
	}

	/*
#if SIG_ENABLE_BOOST
	//コンテナに格納された全文字列を結合して1つの文字列に(delimiterで区切り指定)
	template < class T, template < class T, class = std::allocator<T >> class Container >
	inline std::string CatStr(Container<T> const& container, std::string delimiter = "")
	{
	std::string tmp;

	for (auto const& src : container){
	tmp += (boost::format("%1%%2%") % src % delimiter).str();
	}
	return std::move(tmp);
	}
#endif
	*/

	//コンテナに格納された全文字列を結合して1つの文字列に(delimiterで区切り指定)
	template < class T, template < class T_, class = std::allocator<T_>> class Container >
	inline std::string CatStr(Container<T> const& container, std::string delimiter = std::string())
	{
		if (container.empty()) return std::string();
		std::ostringstream ostream;

		ostream << *container.begin();
		for (auto it = ++container.begin(), end = container.end(); it != end; ++it){
			ostream << delimiter << *it;
		}
		return ostream.str();
	}

	template < class T, template < class T_, class = std::allocator<T_>> class Container >
	inline std::wstring CatWStr(Container<T> const& container, std::wstring delimiter = std::wstring())
	{
		if (container.empty()) return std::wstring();
		std::wostringstream ostream;

		ostream << *container.begin();
		for (auto it = ++container.begin(), end = container.end(); it != end; ++it){
			ostream << delimiter << *it;
		}
		return ostream.str();
	}



	//UTF-16 to Shift-JIS
	inline std::string WSTRtoSTR(const std::wstring &src)
	{
		size_t mbs_size = src.length() * MB_CUR_MAX + 1;
		if (mbs_size < 2 || src == L"\0") return std::string();
		char *mbs = new char[mbs_size];
		size_t num;

		wcstombs_s(&num, mbs, mbs_size, src.c_str(), src.length() * MB_CUR_MAX + 1);
		std::string dest(mbs);
		delete[] mbs;

		return std::move(dest);
	}

	template < template < class T_, class = std::allocator<T_>> class Container >
	Container<std::string> WSTRtoSTR(Container<std::wstring> const& strvec)
	{
		Container<std::string> result;
		for (auto const& str : strvec) result.push_back(WSTRtoSTR(str));
		return std::move(result);
	}

	//Shift-JIS to UTF-16
	inline std::wstring STRtoWSTR(const std::string &src)
	{
		size_t wcs_size = src.length() + 1;
		if (wcs_size < 2 || src == "\0") return std::wstring();
		//std::cout << src << std::endl;
		wchar_t *wcs = new wchar_t[wcs_size];
		size_t num;

		mbstowcs_s(&num, wcs, wcs_size, src.c_str(), src.length() + 1);
		std::wstring dest(wcs);
		delete[] wcs;

		return std::move(dest);
	}

	template < template < class T_, class = std::allocator<T_>> class Container >
	Container<std::wstring> STRtoWSTR(Container<std::string> const& strvec)
	{
		Container<std::wstring> result;
		for (auto const& str : strvec) result.push_back(STRtoWSTR(str));
		return std::move(result);
	}

	//全角・半角文字の置換処理を行う
	class ZenHanReplace{
		std::unordered_map<wchar_t, wchar_t> _alphabet;			//全角アルファベット -> 半角アルファベット
		std::unordered_map<wchar_t, wchar_t> _number;			//全角数字 -> 半角数字
		std::unordered_map<wchar_t, wchar_t> _katakana;			//半角カタカナ -> 全角カタカナ
		std::unordered_map<std::wstring, wchar_t> _katakana_d;	//(濁音) 半角カタカナ -> 全角カタカナ

	private:
		ZenHanReplace(){
			wchar_t zen1[2] = L"Ａ";
			char han1 = 'A';

			for (uint i = 0; i < 26; ++i, ++zen1[0], ++han1){
				_alphabet[zen1[0]] = han1;
			}

			wchar_t zen2 = L'ａ';
			wchar_t han2 = 'a';
			for (uint i = 0; i < 26; ++i, ++zen2, ++han2){
				_alphabet[zen2] = han2;
			}

			_number[L'０'] = L'0';
			_number[L'１'] = L'1';
			_number[L'２'] = L'2';
			_number[L'３'] = L'3';
			_number[L'４'] = L'4';
			_number[L'５'] = L'5';
			_number[L'６'] = L'6';
			_number[L'７'] = L'7';
			_number[L'８'] = L'8';
			_number[L'９'] = L'9';

			_katakana[L'ｰ'] = L'ー';
			_katakana[L'ｧ'] = L'ァ';
			_katakana[L'ｨ'] = L'ィ';
			_katakana[L'ｩ'] = L'ゥ';
			_katakana[L'ｪ'] = L'ェ';
			_katakana[L'ｫ'] = L'ォ';
			_katakana[L'ｱ'] = L'ア';
			_katakana[L'ｲ'] = L'イ';
			_katakana[L'ｳ'] = L'ウ';
			_katakana[L'ｴ'] = L'エ';
			_katakana[L'ｵ'] = L'オ';

			wchar_t han3 = L'ｶ';
			wchar_t zen3 = L'カ';
			for (uint i = 0; i < 12; ++i, ++han3, zen3 += 2){
				_katakana[han3] = zen3;
			}
			_katakana[L'ｯ'] = L'ッ';
			_katakana[L'ﾃ'] = L'テ';
			_katakana[L'ﾄ'] = L'ト';
			han3 += 2; zen3 += 6;
			for (uint i = 0; i < 6; ++i, ++han3, ++zen3){
				_katakana[han3] = zen3;
			}
			for (uint i = 0; i < 5; ++i, ++han3, zen3 += 3){
				_katakana[han3] = zen3;
			}
			for (uint i = 0; i < 5; ++i, ++han3, ++zen3){
				_katakana[han3] = zen3;
			}
			_katakana[L'ﾔ'] = L'ヤ';
			_katakana[L'ｬ'] = L'ャ';
			_katakana[L'ﾕ'] = L'ユ';
			_katakana[L'ｭ'] = L'ュ';
			_katakana[L'ﾖ'] = L'ヨ';
			_katakana[L'ｮ'] = L'ョ';
			han3 += 3; zen3 += 6;
			for (uint i = 0; i < 5; ++i, ++han3, ++zen3){
				_katakana[han3] = zen3;
			}
			_katakana[L'ﾜ'] = L'ワ';
			_katakana[L'ｦ'] = L'ヲ';
			_katakana[L'ﾝ'] = L'ン';


			wchar_t han4[3] = L"ｶﾞ";
			wchar_t zen4 = L'ガ';
			for (uint i = 0; i < 12; ++i, ++han4[0], zen4 += 2){
				_katakana_d[han4] = zen4;
			}
			_katakana_d[L"ﾂﾞ"] = L'ヅ';
			_katakana_d[L"ﾃﾞ"] = L'デ';
			_katakana_d[L"ﾄﾞ"] = L'ド';
			han4[0] += 8; zen4 += 12;
			for (uint i = 0; i < 5; ++i, ++han4[0], zen4 += 3){
				_katakana_d[han4] = zen4;
			}
			wchar_t han5[3] = L"ﾊﾟ";
			wchar_t zen5 = L'パ';
			for (uint i = 0; i < 5; ++i, ++han5[0], zen5 += 3){
				_katakana_d[han5] = zen5;
			}
		}

		ZenHanReplace(const ZenHanReplace&) = delete;

	public:
		static ZenHanReplace& GetInstance(){
			static ZenHanReplace instance;
			return instance;
		}

		void Alphabet_Zen2Han(std::wstring& sentence) const{
			for (auto& c : sentence){
				if (_alphabet.count(c)) c = _alphabet.at(c);
			}
		}

		void Number_Zen2Han(std::wstring& sentence) const{
			for (auto& c : sentence){
				if (_number.count(c)) c = _number.at(c);
			}
		}

		void Katakana_Han2Zen(std::wstring& sentence) const{
			for (unsigned i = 1; i < sentence.size(); ++i){
				if (sentence[i] == L'ﾞ'){
					wchar_t tmp[3] = { sentence[i - 1], sentence[i], L'\0' };
					auto ttmp = std::wstring(tmp);
					if (_katakana_d.count(ttmp)) sentence.replace(i - 1, 2, 1, _katakana_d.at(ttmp));
				}
			}
			for (auto& c : sentence){
				if (_katakana.count(c)) c = _katakana.at(c);
			}
		}
	};

}


#endif