#include "file_test.h"
#include "../lib/string.hpp"
#include "../lib/functional.hpp"

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる
//処理方法の優先順位は SIG_MSVC_ENV(windows.h使用) > SIG_ENABLE_BOOOST(boost::filesystem使用)

using TVec = std::vector<std::string>;
using TVecw = std::vector<std::wstring>;

#if SIG_MSVC_ENV
	auto raw_pass = L"../SigUtil/example/test_file";
#else
	auto raw_pass = "../SigUtil/example/test_file";
#endif

void GetDirectoryNamesTest()
{
	auto pass = sig::DirpassTailModify(raw_pass, true);

#if SIG_MSVC_ENV || SIG_ENABLE_BOOST

	auto file_names = sig::GetFileNames(pass, false);
	auto text_file_names = sig::GetFileNames(pass, false, L".txt");
	auto old_text_file_names = sig::GetFileNames(pass, false, L".old.txt");
	auto hidden_text_file_names = sig::GetFileNames(pass, true, L".txt");

#if SIG_ENABLE_BOOST
	std::cout << std::endl  << "[all visible files]"<< std::endl;
	if(file_names){
		for (auto fn : *file_names) std::wcout << fn << std::endl;
	}
	std::cout << std::endl << "[all .txt files]" << std::endl;
	if(text_file_names){
		for (auto fn : *text_file_names) std::wcout << fn << std::endl;
	}
	std::cout << std::endl << "[all .old.txt files]" << std::endl;
	if(old_text_file_names){
		for (auto fn : *old_text_file_names) std::wcout << fn << std::endl;
	}
	std::cout << std::endl << "[all hidden files]" << std::endl;
	if(hidden_text_file_names){
		for (auto fn : *hidden_text_file_names) std::wcout << fn << std::endl;
	}
#else
	std::cout << std::endl << "[all visible files]" << std::endl;
	for (auto fn : file_names) std::wcout << fn << std::endl;
	std::cout << std::endl << "[all .txt files]" << std::endl;
	for (auto fn : text_file_names) std::wcout << fn << std::endl;
	std::cout << std::endl << "[all .old.txt files]" << std::endl;
	for (auto fn : old_text_file_names) std::wcout << fn << std::endl;
	std::cout << std::endl << "[all hidden files]" << std::endl;
	for (auto fn : hidden_text_file_names) std::wcout << fn << std::endl;
#endif

	auto folder_names = sig::GetFolderNames(pass, false);
	auto hidden_folder_names = sig::GetFolderNames(pass, true);

#if SIG_ENABLE_BOOST
	std::cout << std::endl << "[all visible folders]" << std::endl;
	if (folder_names){
		for (auto fn : *folder_names) std::wcout << fn << std::endl;
	}
	std::cout << std::endl << "[all hidden folders]" << std::endl;
	if (hidden_folder_names){
		for (auto fn : *hidden_folder_names) std::wcout << fn << std::endl;
	}
#else
	std::cout << std::endl << "[all visible folders]" << std::endl;
	for (auto fn : folder_names) std::wcout << fn << std::endl;
	std::cout << std::endl << "[all hidden folders]" << std::endl;
	for (auto fn : hidden_folder_names) std::wcout << fn << std::endl;
#endif
#else
	std::cout << "this OS is not support. please include boost if any." << std::endl; 
#endif
}

void FileSaveLoadTest()
{
	auto pass = sig::DirpassTailModify(raw_pass, true);

#if SIG_MSVC_ENV
	auto fpass1 = pass + L"test.txt";
	auto fpass2 = pass + L"test2.txt";
	auto fpass3 = pass + L"test3.txt";
	auto fpass4 = pass + L"test4.txt";
	auto fpass5 = pass + L"test5.txt";

	std::wcout << fpass1 << std::endl << std::endl;
#else
	auto fpass1 = pass + "test.txt";
	auto fpass2 = pass + "test2.txt";
	auto fpass3 = pass + "test3.txt";
	auto fpass4 = pass + "test4.txt";
	auto fpass5 = pass + "test5.txt";

	std::cout << fpass1 << std::endl << std::endl;
#endif


	std::vector<std::wstring> blghost_text1{
		L"O.K.",
		L"CLOSE INITIALIZE SEQUENCE.",
		L"AND",
		L"BIGIN TO LOG ON",
		L"B.L.GHOST STRATEGY RECORDER."
	};

	std::list<std::string> blghost_text2{
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
	sig::FileClear(fpass1);
	sig::FileClear(fpass5);

	//ofstreamを渡して保存
	std::wofstream ofs(fpass1, std::ios::out | std::ios::app);
	sig::SaveLine(L"test write 0", ofs);		//1行保存
	sig::SaveLine(blghost_text1, ofs);		//全行保存
	ofs.close();

	//以下 かんたん保存♪

	//1行保存（上書き）
	sig::SaveLine(L"test write 壱", fpass2);
	//1行保存（追記）
	sig::SaveLine(L"test write 弐", fpass2, sig::WriteMode::append);
	//全行保存（上書き）
	sig::SaveLine(blghost_text2, fpass3);

	//数値データの保存（上書き、1行ずつ保存）
	auto set_num = std::set<int, std::less<int>>{1, 2, 3, 4, 5};
	sig::SaveNum(set_num, fpass4);

	//数値データの保存（追記、カンマ分けで保存）
	auto uset_num = std::unordered_set<double>{1.1, 2.2, 3.3};
	sig::SaveNum(uset_num, fpass5, sig::WriteMode::append, ",");


	//以下 かんたん読み込み♪

#if SIG_ENABLE_BOOST
	auto read1 = sig::ReadLine<std::string>(fpass1);
	auto read2 = sig::ReadLine<std::wstring, std::list<std::wstring>>(fpass2);
	auto read_num1 = sig::ReadNum<int>(fpass4);
	auto read_num2 = sig::ReadNum<double, std::set<double>>(fpass5, ",");

	if (read1){
		auto test1 = sig::Merge(TVecw{L"test write 0"}, blghost_text1);
		sig::ZipWith([&](std::string s1, std::wstring s2){ assert(sig::STRtoWSTR(s1) == s2); return 0; }, *read1, test1);
	}
	if (read2){
		sig::ZipWith([](std::wstring s1, std::wstring s2){ assert(s1 == s2); return 0; }, *read2, TVecw{ L"test write 壱", L"test write 弐" });
	}
	if (read_num1){
		sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, *read_num1, set_num);
	}
	if (read_num2){
		//unorderedなのでdouble値の合計で判断
		assert(std::accumulate(uset_num.begin(), uset_num.end(), 0.0) == std::accumulate(read_num2->begin(), read_num2->end(), 0.0));
	}
#else
	std::vector<std::string> read1;
	std::list<std::wstring> read2;
	std::vector<int> read_num1;
	std::set<double> read_num2;

	sig::ReadLine(read1, fpass1);
	sig::ReadLine(read2, fpass2);
	sig::ReadNum(read_num1, fpass4);
	sig::ReadNum(read_num2, fpass5, ",");
	
	auto test1 = sig::Merge(TVecw{L"test write 0"}, blghost_text1);
	sig::ZipWith([&](std::string s1, std::wstring s2){ assert(sig::STRtoWSTR(s1) == s2); return 0; }, read1, test1);

	sig::ZipWith([](std::wstring s1, std::wstring s2){ assert(s1 == s2); return 0; }, read2, TVecw{ L"test write 壱", L"test write 弐" });

	sig::ZipWith([](int v1, int v2){ assert(v1 == v2); return 0; }, read_num1, set_num);
	
	assert(std::accumulate(uset_num.begin(), uset_num.end(), 0.0) == std::accumulate(read_num2.begin(), read_num2.end(), 0.0));
#endif
}