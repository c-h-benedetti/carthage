#include "File.hpp"

int File::load(){
	// std::cerr << "Launching file: " << this->block.name << std::endl;
	return 0;
}


int File::open(){
	return this->load();
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


File::File(FileSystem& fs): FSObject(fs){}

File::File(const FSObject& obj): FSObject(obj){}

File::File(FileSystem& fs, const FSBlock& bck, const FSPos& pos): FSObject(fs, bck, pos){}