#include <iostream>
#include "FileSystem.hpp"

int main(int argc, char* argv[], char* env[]){

	std::string path = "/home/clement/Desktop/file_system/";
	FileSystem& f = FileSystem::file_system(path);
	
	std::cout << (f.folders()[0].name()) << std::endl;
	std::cout << (f.folders()[0].folders()[0].name()) << std::endl;
	std::cout << (f.folders()[0].folders()[0].files()[0].name()) << std::endl;

	for (const File& file : f.files()){
		std::cout << (int)file.get_type() << std::endl;
	}

	FileSystem::terminate();

	return 0;
}

/*

[ ] Make a version that enables to work on several disks, at least on server side
[X] Files are heavy on disk but not numerous so the complete representation of the project can be loaded

*/