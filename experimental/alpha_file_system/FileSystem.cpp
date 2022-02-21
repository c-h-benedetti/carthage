#include "FileSystem.hpp"
#include <iostream>

FileSystem* FileSystem::ptr_file_system = nullptr;

FileSystem::FileSystem() : Folder(){
	this->detect_file_system();
}

FileSystem::FileSystem(const std::string& s) : Folder(s){
	this->detect_file_system();
}

FileSystem::~FileSystem(){}

FileSystem& FileSystem::file_system(){
	if (FileSystem::ptr_file_system == nullptr){
		FileSystem::ptr_file_system = new FileSystem{};
	}
	if (!FileSystem::ptr_file_system->exists()){
		std::cerr << "Failed to initialize file system handler" << std::endl;
		throw 10;
	}
	return *(FileSystem::ptr_file_system);
}

FileSystem& FileSystem::file_system(const std::string& s){
	if (FileSystem::ptr_file_system == nullptr){
		FileSystem::ptr_file_system = new FileSystem{s};
	}
	else{
		std::cerr << "An instance of the singleton already exists" << std::endl;
		throw 20;
	}
	return *(FileSystem::ptr_file_system);
}

void FileSystem::terminate(){
	if (FileSystem::ptr_file_system != nullptr){
		delete FileSystem::ptr_file_system;
	}
}

void FileSystem::detect_file_system(){
	std::string s_path = this->path;
	size_t i = s_path.length();

	for(const fs::directory_entry& dir_entry: fs::directory_iterator{this->path}){
		std::string s1 = dir_entry.path();
        std::string name = s1.substr(i, s1.length());
        if (dir_entry.is_regular_file()){
        	this->_files.push_back(File(name, dir_entry));
        }
        else if (dir_entry.is_directory()){
        	this->_folders.push_back(Folder(name, dir_entry));
        	this->_folders.back().detect_file_system_rec(dir_entry);
        }
        else{
        	std::cerr << "Object not handled: " << name << std::endl;
        }
	}
}

void FileSystem::save_state() const{
	
}


/*

[X] The constructor must take a std::string as argument
[ ] Change the constructor to get the root init from the environment

*/