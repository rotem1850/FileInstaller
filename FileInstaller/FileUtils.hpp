#pragma once
#include <iostream>

namespace FileUtils {
	void copy_file(std::wstring const &source_file_path, std::wstring const &dest_file_path);
	void delete_file(std::wstring const &file_path);
	void create_directory(std::wstring const &directory_path);
	void delete_directory(std::wstring const &directory_path);
	bool is_path_exists(std::wstring const &path);

	/*
	This function change the directory of a file path to another directory.
	e.g: For "C:\\test\\end.txt" with new_dir="C:\\test2" the result will be "C:\\test2\\end.txt"
	*/
	std::wstring change_file_path_directory(std::wstring const &source_file_path,
											std::wstring const &new_directory);


}