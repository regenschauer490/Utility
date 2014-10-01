/*
Copyright© 2014 Akihiro Nishimura

This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/

#ifndef SIG_UTIL_REPLACE_HPP
#define SIG_UTIL_REPLACE_HPP

#include "../sigutil.hpp"
#include <unordered_map>


/// \file manipulate.hpp 文字列置換

namespace sig
{

/// 全角・半角文字の置換処理を行う
class ZenHanReplace
{
	std::unordered_map<wchar_t, wchar_t> alphabet_;			// 全角アルファベット -> 半角アルファベット
	std::unordered_map<wchar_t, wchar_t> number_;			// 全角数字 -> 半角数字
	std::unordered_map<wchar_t, wchar_t> katakana_;			// 半角カタカナ -> 全角カタカナ
	std::unordered_map<std::wstring, wchar_t> katakana_d_;	// (濁音) 半角カタカナ -> 全角カタカナ

private:
	ZenHanReplace(){
		wchar_t zen1[2] = L"Ａ";
		char han1 = 'A';

		for (uint i = 0; i < 26; ++i, ++zen1[0], ++han1){
			alphabet_[zen1[0]] = han1;
		}

		wchar_t zen2 = L'ａ';
		wchar_t han2 = 'a';
		for (uint i = 0; i < 26; ++i, ++zen2, ++han2){
			alphabet_[zen2] = han2;
		}

		number_[L'０'] = L'0';
		number_[L'１'] = L'1';
		number_[L'２'] = L'2';
		number_[L'３'] = L'3';
		number_[L'４'] = L'4';
		number_[L'５'] = L'5';
		number_[L'６'] = L'6';
		number_[L'７'] = L'7';
		number_[L'８'] = L'8';
		number_[L'９'] = L'9';
		katakana_[L'ｰ'] = L'ー';
		katakana_[L'ｧ'] = L'ァ';
		katakana_[L'ｨ'] = L'ィ';
		katakana_[L'ｩ'] = L'ゥ';
		katakana_[L'ｪ'] = L'ェ';
		katakana_[L'ｫ'] = L'ォ';
		katakana_[L'ｱ'] = L'ア';
		katakana_[L'ｲ'] = L'イ';
		katakana_[L'ｳ'] = L'ウ';
		katakana_[L'ｴ'] = L'エ';
		katakana_[L'ｵ'] = L'オ';

		wchar_t han3 = L'ｶ';
		wchar_t zen3 = L'カ';
		for (uint i = 0; i < 12; ++i, ++han3, zen3 += 2){
			katakana_[han3] = zen3;
		}
		katakana_[L'ｯ'] = L'ッ';
		katakana_[L'ﾃ'] = L'テ';
		katakana_[L'ﾄ'] = L'ト';
		han3 += 2; zen3 += 6;
		for (uint i = 0; i < 6; ++i, ++han3, ++zen3){
			katakana_[han3] = zen3;
		}
		for (uint i = 0; i < 5; ++i, ++han3, zen3 += 3){
			katakana_[han3] = zen3;
		}
		for (uint i = 0; i < 5; ++i, ++han3, ++zen3){
			katakana_[han3] = zen3;
		}
		katakana_[L'ﾔ'] = L'ヤ';
		katakana_[L'ｬ'] = L'ャ';
		katakana_[L'ﾕ'] = L'ユ';
		katakana_[L'ｭ'] = L'ュ';
		katakana_[L'ﾖ'] = L'ヨ';
		katakana_[L'ｮ'] = L'ョ';
		han3 += 3; zen3 += 6;
		for (uint i = 0; i < 5; ++i, ++han3, ++zen3){
			katakana_[han3] = zen3;
		}
		katakana_[L'ﾜ'] = L'ワ';
		katakana_[L'ｦ'] = L'ヲ';
		katakana_[L'ﾝ'] = L'ン';

		wchar_t han4[3] = L"ｶﾞ";
		wchar_t zen4 = L'ガ';
		for (uint i = 0; i < 12; ++i, ++han4[0], zen4 += 2){
			katakana_d_[han4] = zen4;
		}
		katakana_d_[L"ﾂﾞ"] = L'ヅ';
		katakana_d_[L"ﾃﾞ"] = L'デ';
		katakana_d_[L"ﾄﾞ"] = L'ド';
		han4[0] += 8; zen4 += 12;
		for (uint i = 0; i < 5; ++i, ++han4[0], zen4 += 3){
			katakana_d_[han4] = zen4;
		}
		wchar_t han5[3] = L"ﾊﾟ";
		wchar_t zen5 = L'パ';
		for (uint i = 0; i < 5; ++i, ++han5[0], zen5 += 3){
			katakana_d_[han5] = zen5;
		}
	}

	ZenHanReplace(const ZenHanReplace&) = delete;

public:
	/**
		インスタンスの参照を取得
	*/
	static ZenHanReplace& getInstance(){
		static ZenHanReplace instance;
		return instance;
	}

	///	アルファベット 全角 -> 半角
	void alphabet_zen2han(std::wstring& sentence) const{
		for (auto& c : sentence){
			if (alphabet_.count(c)) c = alphabet_.at(c);
		}
	}
	
	///	アルファベット 半角 -> 全角
	void alphabet_han2zen(std::wstring& sentence) const{
		for (auto& c : sentence){
			for (auto const& v : alphabet_){
				if (v.second == c) c = v.first;
			}
		}
	}
	
	///	数字 全角 -> 半角
	void number_zen2han(std::wstring& sentence) const{
		for (auto& c : sentence){
			if (number_.count(c)) c = number_.at(c);
		}
	}

	///	数字 半角 -> 全角
	void number_han2zen(std::wstring& sentence) const{
		for (auto& c : sentence){
			for (auto const& v : number_){
				if (v.second == c) c = v.first;
			}
		}
	}
	
	///	カタカナ 全角 -> 半角
	void katakana_zen2han(std::wstring& sentence) const{
		for (uint i = 0; i<sentence.size(); ++i){
			for (auto const& v : katakana_d_){
				if (v.second == sentence[i]){
					sentence.replace(i, 1, v.first);
				}
			}
		}
		for (auto& c : sentence){
			for (auto const& v : katakana_){
				if (v.second == c) c = v.first;
			}
		}
	}

	
	///	カタカナ 半角 -> 全角
	void katakana_han2zen(std::wstring& sentence) const{
		for (unsigned i = 1; i < sentence.size(); ++i){
			if (sentence[i] == L'ﾞ' || sentence[i] == L'ﾟ'){
				auto ttmp = std::wstring(sentence.substr(i - 1, 2));
				if (katakana_d_.count(ttmp)){
					sentence.replace(i - 1, 2, 1, katakana_d_.at(ttmp));
					--i;
				}
			}
		}
		for (auto& c : sentence){
			if (katakana_.count(c)) c = katakana_.at(c);
		}
	}
};

}
#endif