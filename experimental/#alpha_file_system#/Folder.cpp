#include "Folder.hpp"
#include <iostream>

Folder::Folder() : FileSystemObject(){

}

Folder::Folder(const std::string& s) : FileSystemObject(s){

}

Folder::Folder(const std::string& s, const fs::directory_entry& d) : Folder(s){

}

void Folder::detect_file_system_rec(const fs::directory_entry& d){
	std::string s_path = d.path();
	size_t i = s_path.length();

	for(const fs::directory_entry& dir_entry: fs::directory_iterator{d.path()}){
		std::string s1 = dir_entry.path();
        std::string name = s1.substr(i+1, s1.length());
        if (dir_entry.is_regular_file()){
        	this->_files.push_back(File(name));
        }
        else if (dir_entry.is_directory()){
        	this->_folders.push_back(Folder(name));
        	this->_folders.back().detect_file_system_rec(dir_entry);
        }
        else{
        	std::cerr << "Object not handled: " << name << std::endl;
        }
	}
}