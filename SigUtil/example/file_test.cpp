#include "file_test.h"
#include "debug.hpp"
#include "../lib/string.hpp"

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる
//処理方法の優先順位は SIG_MSVC_ENV(windows.h使用) > SIG_ENABLE_BOOOST(boost::filesystem使用)

using TVec = std::vector<std::string>;
using TVecw = std::vector<std::wstring>;

auto raw_pass = SIG_TO_FPSTR("../SigUtil/example/test_file");

void GetDirectoryNamesTest()
{
	const auto pass = sig::modify_dirpass_tail(raw_pass, true);

#if SIG_MSVC_ENV || SIG_ENABLE_BOOST

	const auto file_names = sig::get_file_names(pass, false);
	const auto text_file_names = sig::get_file_names(pass, false, L".txt");
	const auto old_text_file_names = sig::get_file_names(pass, false, L".old.txt");
	const auto hidden_text_file_names = sig::get_file_names(pass, true, L".txt");

#if SIG_LINUX_ENV
	const std::set<std::wstring> t_text{
		L"test.txt",
		L"test2.txt",
		L"test3.txt",
		L"test4.txt",
		L"test5.txt",
		L"test6.txt",
		L"histgram1.txt",
		L"histgram2.txt",
		L"shift_jis.txt",
		L"utf8.txt"
	};
	const std::set<std::wstring> t_hidden{ L".hidden file linux.txt" };
	const std::set<std::wstring> t_old_text{ L"test.old.txt" };
	const std::set<std::wstring> t_noextension{ L"dummy" };

#else
	const std::set<std::wstring> t_text{
		L"test.txt",
		L"test2.txt",
		L"test3.txt",
		L"test4.txt",
		L"test5.txt",
		L"test6.txt",
		L"histgram1.txt",
		L"histgram2.txt",
		L"shift_jis.txt",
		L"utf8.txt",
		L".hidden file linux.txt",
		L"long_text.txt"
	};
	const std::set<std::wstring> t_hidden{};
	const std::set<std::wstring> t_old_text{ L"test.old.txt" };
	const std::set<std::wstring> t_noextension{ L"dummy" };
	
#endif

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	std::cout << std::endl  << "[all visible files]"<< std::endl;
	auto all_visible = sig::fromJust(file_names);
	auto t_all_visible = sig::merge(t_text, sig::merge(t_old_text, t_noextension));

	assert(all_visible.size() == t_all_visible.size());
	for (auto fn : all_visible){
		std::wcout << fn << std::endl;
		assert(t_all_visible.count(fn));
	}
	
	std::cout << std::endl << "[all .txt files]" << std::endl;
	auto text_visible = sig::fromJust(text_file_names);
	auto t_text_visible = sig::merge(t_text, t_old_text);

	assert(text_visible.size() == t_text_visible.size());
	for (auto fn : text_visible){
		std::wcout << fn << std::endl;
		assert(t_text_visible.count(fn));
	}
	
	std::cout << std::endl << "[all .old.txt files]" << std::endl;
	auto old_text_visible = sig::fromJust(old_text_file_names);
	auto t_old_text_visible = t_old_text;
	assert(old_text_visible.size() == t_old_text.size());

	for (auto fn : old_text_visible){
		std::wcout << fn << std::endl;
		assert(t_old_text_visible.count(fn));
	}
	
	std::cout << std::endl << "[all hidden files]" << std::endl;
	for (auto fn : sig::fromJust(hidden_text_file_names)){
		std::wcout << fn << std::endl;
#if SIG_LINUX_ENV
		assert(sig::fromJust(hidden_text_file_names).size() == t_hidden.size());
		assert(t_hidden.count(fn));
#endif
	}
#endif

	const auto folder_names = sig::get_folder_names(pass, false);
	const auto hidden_folder_names = sig::get_folder_names(pass, true);

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	std::cout << std::endl << "[all visible folders]" << std::endl;
	for (auto fn : sig::fromJust(folder_names)) std::wcout << fn << std::endl;
	
	std::cout << std::endl << "[all hidden folders]" << std::endl;
	for (auto fn : sig::fromJust(hidden_folder_names)) std::wcout << fn << std::endl;
#endif
#else
	std::cout << "I don't support this environment. please include boost if any." << std::endl; 
#endif
	std::cout << std::endl;
}


void FileSaveLoadTest()
{
	const auto pass = sig::modify_dirpass_tail(raw_pass, true);

	const auto fpass1 = pass + SIG_TO_FPSTR("test.txt");
	const auto fpass2 = pass + SIG_TO_FPSTR("test2.txt");
	const auto fpass3 = pass + SIG_TO_FPSTR("test3.txt");
	const auto fpass4 = pass + SIG_TO_FPSTR("test4.txt");
	const auto fpass5 = pass + SIG_TO_FPSTR("test5.txt");
	const auto fpass6 = pass + SIG_TO_FPSTR("test6.txt");

	const std::vector<std::wstring> blghost_text1{
		L"O.K.",
		L"CLOSE INITIALIZE SEQUENCE.",
		L"AND",
		L"BIGIN TO LOG ON",
		L"B.L.GHOST STRATEGY RECORDER."
	};

	const std::list<std::string> blghost_text2{
		"HAIL TO YOU,MY FELLOW.",
		"I DON'T KNOW YOUR INFERNAL DAYS.",
		"HOWEVER.",
		"THERE IS NO CAUSE FOR CONCERN",
		"'CAUSE I'M ALWAYS CLOSE TO YOU.",
		"",
		"SO,YOUR PAIN IS MINE.",
		"KEEP YOUR DIGNITY."
	};	

	//既存の内容のクリア
	sig::clear_file(fpass1);
	sig::clear_file(fpass4);

	//ofstreamを渡して保存
	std::wofstream ofs(fpass1, std::ios::out | std::ios::app);
	sig::save_line(L"test write 0", ofs);		//1行保存
	sig::save_line(blghost_text1, ofs);		//全行保存
	ofs.close();

/* 書き込み */

	//1行保存（上書き）
	sig::save_line(L"test write 壱", fpass2);
	//1行保存（追記）
	sig::save_line(L"test write 弐", fpass2, sig::WriteMode::append);
	//全行保存（上書き）
	sig::save_line(blghost_text2, fpass3);

	//数値列の保存（追記、1行ずつ保存）
	const auto list_num = std::list<double>{-1.1, -2.2, -3.3};
	const auto uset_num = std::unordered_set<double>{1.1, 2.2, 3.3};
	sig::save_num(list_num, fpass4, "\n", sig::WriteMode::overwrite);
	sig::save_num(uset_num, fpass4, "\n", sig::WriteMode::append);

	//数値行列の保存 (上書き、各行カンマ区切りで保存)
	const sig::array<sig::array<int, 3>, 3> mat = {
		{ 1, 2, 3 },
		{ 4, 5, 6 },
		{ 7, 8, 9 }
	};
	sig::save_num(mat, fpass5, ",");


/* 読み込み */

#if SIG_ENABLE_BOOST && SIG_USE_OPTIONAL
	const auto read1 = sig::load_line(fpass1);
	const auto read2 = sig::load_line<std::wstring, std::list<std::wstring>>(fpass2);
	const auto load_num = sig::load_num<double, std::set<double>>(fpass4);
	const auto read_mat = sig::load_num2d<int>(fpass5, ",");

	if (read1){
		const auto test1 = sig::merge(TVecw{ L"test write 0" }, blghost_text1);
		sig::assert_foreach(sig::Identity(), sig::str_to_wstr(sig::fromJust(read1)), test1);
	}
	if (read2){
		sig::assert_foreach(sig::Identity(), sig::fromJust(read2), TVecw{ L"test write 壱", L"test write 弐" });
	}
	if (load_num){
		const auto test = std::accumulate(list_num.begin(), list_num.end(), 0.0) + std::accumulate(uset_num.begin(), uset_num.end(), 0.0);
		//保存前がunorderedで順不同となるので、読み取り後のdouble値の合計と一致するかで判断
		assert( sig::equal(std::accumulate(load_num->begin(), load_num->end(), 0.0), test) );
	}
	if (read_mat){
		for(unsigned i=0; i<read_mat->size(); ++i){
			sig::assert_foreach(sig::Identity(), sig::fromJust(read_mat)[i], mat[i]);
		}
	}
#endif
	std::vector<std::string> read3;
	std::list<std::wstring> read4;
	std::set<double> read_num2;
	std::vector<std::vector<int>> read_mat2;

	sig::load_line(read3, fpass1);
	sig::load_line(read4, fpass2);
	sig::load_num(read_num2, fpass4);
	sig::load_num2d(read_mat2, fpass5, ",");
	
	const auto test1 = sig::merge(TVec{"test write 0"}, sig::wstr_to_str(blghost_text1));
	sig::assert_foreach(sig::Identity(), read3, test1);

	sig::assert_foreach(sig::Identity(), read4, TVecw{ L"test write 壱", L"test write 弐" });

	const auto test2 = std::accumulate(list_num.begin(), list_num.end(), 0.0) + std::accumulate(uset_num.begin(), uset_num.end(), 0.0);
	//保存前がunorderedで順不同となるので、読み取り後のdouble値の合計と一致するかで判断
	assert(sig::equal(std::accumulate(read_num2.begin(), read_num2.end(), 0.0), test2));
	
	for (unsigned i = 0; i<read_mat2.size(); ++i){
		sig::assert_foreach(sig::Identity(), read_mat2[i], mat[i]);
	}


	struct Test{
		double a;
		Test(double v, double p) : a(std::pow(v, p)){}
	};

	std::vector<Test> input;

	sig::load_line(
		input,
		fpass6,
		[&](std::string&& s)->Test{
			auto tmp = sig::split(s, ",");		// example: s = "2.73,0.001"
			return Test(std::stod(tmp[0]), std::stod(tmp[1]));
		}
	);
}
