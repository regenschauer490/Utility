#ifndef __SIG_UTIL_STRING__
#define __SIG_UTIL_STRING__

#include "sigutil.hpp"
#include <string>


/* 文字列処理関連 */

namespace sig{

	//HTML風にタグをエンコード・デコードする
	//例：　<TAG>text<TAG>
	template <class String>
	class TagDealer
	{
		const String _tel;
		const String _ter;

	public:
		TagDealer(String tag_encloser_left, String tag_encloser_right) : _tel(tag_encloser_left), _ter(tag_encloser_right){};

		String Encode(String const& src, String const& tag) const{
			auto tag_str = _tel + tag + _ter;
			return tag_str + src + tag_str;
		}

		template < template < class T, class Allocator = std::allocator<T>> class Container >
		String Encode(Container<String> const& src, Container<String> const& tag) const;

		typename MaybeReturn<String>::type Decode(String const& src, String const& tag) const{
			auto tag_str = _tel + tag + _ter;
			auto parse = Split(src, tag_str, false);
			return parse.size() < 2 ? Nothing(EmptyString<String>::value) : typename MaybeReturn<String>::type(parse[1]);
		}

		template < template < class T, class Allocator = std::allocator<T >> class Container >
		typename MaybeReturn<Container<String>>::type Decode(String const& src, Container<String> const& tag) const;
	};

	template <class String>
	template < template < class T, class Allocator = std::allocator<T >> class Container >
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
	template < template < class T, class Allocator = std::allocator<T >> class Container >
	typename MaybeReturn<Container<String>>::type TagDealer<String>::Decode(String const& src, Container<String> const& tag) const
	{
		Container<String> result;
		for (auto const& e : tag){
			if (auto d = Decode(src, e)) result.push_back(*d);
		}
		return result.empty() ? nothing : maybe<Container<String>>(std::move(result));
	}


	//文字列をある文字を目印に分割する
	template < class String, template <class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<String> Split(String src, typename std::common_type<String>::type const& delim, bool ignore_blank = true)
	{
		Container<String> result;
		int const mag = delim.size();
		int cut_at;

		while ((cut_at = src.find(delim)) != src.npos){
			if (!ignore_blank || cut_at > 0) result.push_back(src.substr(0, cut_at));
			src = src.substr(cut_at + mag);
		}
		if (!ignore_blank || src.length() > 0){
			result.push_back(src);
		}

		return std::move(result);
	}

	template < template <class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<std::string> Split(char const* src, char const* delim)
	{
		return Split<std::string, Container>(std::string(src), delim);
	}

	template < template < class T_, class = std::allocator<T_>> class Container = std::vector >
	Container<std::wstring> Split(wchar_t const* src, wchar_t const* delim)
	{
		return Split<std::wstring, Container>(std::wstring(src), delim);
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

		for (auto const& src : container){
			ostream << src << delimiter;
		}
		return ostream.str();
	}

	template <class STRING>
	struct Map2Regex{
		typedef void type;
	};
	template <>
	struct Map2Regex<std::string>{
		typedef std::regex type;
	};
	template <>
	struct Map2Regex<std::wstring>{
		typedef std::wregex type;
	};

	template <class STRING>
	struct Map2Smatch{
		typedef void type;
	};
	template <>
	struct Map2Smatch<std::string>{
		typedef std::smatch type;
	};
	template <>
	struct Map2Smatch<std::wstring>{
		typedef std::wsmatch type;
	};


	//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープする
	template <class String>
	String RegexEscaper(String expression)
	{
		static const std::wregex escape_reg(L"([(){}\\[\\]|^?$.+*\\\\])");
		return std::regex_replace(expression, escape_reg, L"\\$1");
	}

	template <class String>
	typename Map2Regex<String>::type RegexMaker(String const& expression)
	{
		return typename Map2Regex<String>::type(RegexEscaper(expression));
	}

#if ENABLE_BOOST

	//std::regex_search のラッパ関数。
	//return -> maybe ? [マッチした箇所の順番][マッチ内の参照の順番. 0は全文, 1以降は参照箇所] : nothing
	//例：
	//src = "test tes1 tes2"
	//expression = std::regex("tes(\\d)")
	//return -> [[tes, 1], [tes, 2]]
	template < class String, template<class T, class = std::allocator<T>> class Container = std::vector >
	maybe< Container< Container<String>>> RegexSearch(String src, typename Map2Regex<String>::type expression)
	{
		Container<Container<String>> d;
		maybe<Container<Container<String>>> result(d);
		typename Map2Smatch<String>::type match;

		while (std::regex_search(src, match, expression)){
			result->push_back(Container<String>());
			for (auto const& m : match) (*result)[result->size() - 1].push_back(m);
			src = match.suffix().str();
		}

		return result->empty() ? nothing : std::move(result);
	}

	//expressionに含まれる文字に関して、正規表現の特殊文字をエスケープしてから処理（推奨）
	template < class String, template < class T, class = std::allocator<T >> class Container = std::vector >
	maybe< Container< Container<String>>> RegexSearch(String src, String expression)
	{
		return RegexSearch(src, RegexMaker(expression));
	}

#endif

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