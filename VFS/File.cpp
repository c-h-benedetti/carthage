#include "File.hpp"

void File::load(){
	std::cerr << "Launching file: " << this->block.name << std::endl;
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


File::File(FileSystem& fs): FSObject(fs){}

File::File(const FSObject& obj): FSObject(obj){}

File::File(FileSystem& fs, const FSBlock& bck, const FSPos& pos): FSObject(fs, bck, pos){}