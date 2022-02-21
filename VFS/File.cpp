#include "File.hpp"

void File::open(){
	this->load();
}

void File::load(){
	std::cerr << "Launching file: " << this->block.name << std::endl;
}

File::File(FileSystem* fs): FSObject(fs){}

File::File(const FSObject& obj): FSObject(obj){}

File::File(const FSBlock& bck, const FSPos& pos, FileSystem* fs): FSObject(bck, pos, fs){}