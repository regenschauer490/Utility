#include "file_test.hpp"

//SIG_ENABLE_BOOST = 1 の際にはboost::optionalが有効になる
//処理方法の優先順位は SIG_WINDOWS_ENV(windows.h使用) > SIG_ENABLE_BOOOST(boost::filesystem使用)

void GetFileNamesTest()
{
	auto pass = sig::DirpassTailModify(L"C:/boost/boost_1_54_0", true);
	std::wcout << pass << std::endl;		//C:/boost/boost_1_54_0/

	auto file_names = sig::GetFileNames(pass);
	auto text_file_names = sig::GetFileNames(pass, L".txt");
	auto old_text_file_names = sig::GetFileNames(pass, L".old.txt");

#if SIG_ENABLE_BOOST
	if(file_names){
		for (auto fn : *file_names) std::wcout << fn << std::endl;
	}
	std::cout << std::endl;
	if(text_file_names){
		for (auto fn : *text_file_names) std::wcout << fn << std::endl;
	}
	std::cout << std::endl;
	if(old_text_file_names){
		for (auto fn : *old_text_file_names) std::wcout << fn << std::endl;
	}
#else
	for (auto fn : file_names) std::wcout << fn << std::endl;
	std::cout << std::endl;
	for (auto fn : text_file_names) std::wcout << fn << std::endl;
	std::cout << std::endl;
	for (auto fn : old_text_file_names) std::wcout << fn << std::endl;
#endif

}