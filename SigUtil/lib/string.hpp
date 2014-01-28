#ifndef __SIG_UTIL_STRING__
#define __SIG_UTIL_STRING__

#include "sigutil.hpp"
#include <string>
#include <regex>

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
	using String = typename StringId<typename std::decay<T>::type>::type;

	template <class T>
	struct Map2Regex{ typedef void type; };
	template <>
	struct Map2Regex<std::string>{ typedef std::regex type; };
	template <>
	struct Map2Regex<std::wstring>{ typedef std::wregex type; };

	template <class T>
	struct Map2Smatch{ typedef void type; };
	template <>
	struct Map2Smatch<std::string>{ typedef std::smatch type; };
	template <>
	struct Map2Smatch<std::wstring>{ typedef std::wsmatch type; };


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
	auto RegexMaker(T const& expression) ->typename Map2Regex<String<T>>::type
	{
		return typename Map2Regex<String<T>>::type(RegexEscaper(expression));
	}


	//std::regex_search のラッパ関数。
	//return -> maybe ? [マッチした箇所の順番][マッチ内の参照の順番. 0は全文, 1以降は参照箇所] : nothing
	//例：
	//src = "test tes1 tes2"
	//expression = std::regex("tes(\\d)")
	//return -> [[tes1, 1], [tes2, 2]]
	template < class T, template<class T_, class = std::allocator<T_>> class Container = std::vector >
	auto RegexSearch(T src, typename Map2Regex<String<T>>::type const& expression) ->typename MaybeReturn< Container< Container<String<T>>>>::type
	{
		Container<Container<String<T>>> d;
		typename Map2Smatch<String<T>>::type match;
		auto tmp = String<T>(src);

		while (std::regex_search(tmp, match, expression)){
			d.push_back(Container<String<T>>());
			for (auto const& m : match) d[d.size() - 1].push_back(m);
			tmp = match.suffix().str();
		}

		return d.empty() ? Nothing(std::move(d)) : typename MaybeReturn<Container<Container<String<T>>>>::type(std::move(d));
	}

/*
	//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープしてから処理（推奨）
	template < class T, template < class T_, class = std::allocator<T_>> class Container = std::vector >
	auto RegexSearch(T src, T expression) ->typename MaybeReturn< Container< Container<String<T>>>>::type
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
		
		auto Decode(String const& src, String const& tag) ->typename MaybeReturn<String>::type const{
			auto tag_str = tel_ + tag + ter_;
			auto parse = Split(" " + src, tag_str);
			return parse.size() < 2 ? Nothing(String()) : typename MaybeReturn<String>::type(parse[1]);
		}

		template < template < class T_, class Allocator = std::allocator<T_>> class Container >
		String Encode(Container<String> const& src, Container<String> const& tag) const;

#if ENABLE_BOOST
		template < template < class T_, class Allocator = std::allocator<T_>> class Container >
		auto Decode(String const& src, Container<String> const& tag) ->typename MaybeReturn<Container<String>>::type const;
#endif
	};

#if ENABLE_BOOST
	template <class String>
	template < template < class T_, class Allocator = std::allocator<T_>> class Container >
	String TagDealer<String>::Encode(Container<String> const& src, Container<String> const& tag) const
	{
		String result;
		auto size = std::min(src.size(), tag.size());
		for (uint i = 0; i < size; ++i){
			result += Encode(src[i], tag[i]);
		}
		return result;
	}

	template <class String>
	template < template < class T_, class Allocator = std::allocator<T_>> class Container >
	auto TagDealer<String>::Decode(String const& src, Container<String> const& tag) ->typename MaybeReturn<Container<String>>::type const
	{
		Container<String> result;
		for (auto const& e : tag){
			if (auto d = Decode(src, e)) result.push_back(*d);
		}
		return result.empty() ? Nothing(String()) : typename MaybeReturn<Container<String>>::type(std::move(result));
	}
#endif

	//文字列(src)をある文字列(delim)を目印に分割する
	//戻り値のコンテナはデフォルトではvector
	//例：src="one, 2, 参 ", delim="," -> return vector<string>{"one", " 2", " 参 "}
	template < class String, template <class T_, class = std::allocator<T_>> class Container = std::vector >
	auto Split(String src, typename std::common_type<String>::type const& delim) ->Container<typename StringId<String>::type>
	{
		Container<String> result;
		int const mag = delim.size();
		int cut_at;

		while ((cut_at = src.find(delim)) != src.npos){
			if (cut_at > 0) result.push_back(src.substr(0, cut_at));
			src = src.substr(cut_at + mag);
		}
		if (src.length() > 0){
			result.push_back(src);
		}

		return std::move(result);
	}

	template < template <class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<std::string> Split(char const* const src, char const* const delim)
	{
		return Split<std::string, Container>(std::string(src), std::string(delim));
	}

	template < template < class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<std::wstring> Split(wchar_t const* const src, wchar_t const* const delim)
	{
		return Split<std::wstring, Container>(std::wstring(src), std::wstring(delim));
	}

	/*
#if ENABLE_BOOST
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
	inline std::string CatStr(Container<T> const& container, std::string delimiter = "")
	{
		std::ostringstream ostream;

		for (uint i = 0; i < container.size() - 1; ++i){
			ostream << container[i] << delimiter;
		}
		ostream << container[container.size() - 1];
		return ostream.str();
	}

	template < class T, template < class T_, class = std::allocator<T_>> class Container >
	inline std::wstring CatWStr(Container<T> const& container, std::wstring delimiter = L"")
	{
		std::wostringstream ostream;

		for (uint i = 0; i < container.size() - 1; ++i){
			ostream << container[i] << delimiter;
		}
		ostream << container[container.size() - 1];
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

	inline std::vector<std::string> WSTRtoSTR(std::vector<std::wstring> const& strvec)
	{
		std::vector<std::string> result;
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

	inline std::vector<std::wstring> STRtoWSTR(std::vector<std::string> const& strvec)
	{
		std::vector<std::wstring> result;
		for (auto const& str : strvec) result.push_back(STRtoWSTR(str));
		return std::move(result);
	}
}


#endif