#ifndef FILE_BASE_HPP_INCLUDED
#define FILE_BASE_HPP_INCLUDED

#include "FSObject.hpp"

class FileSystem;

class File : public FSObject{

public:

	File();
	File(FileSystem* o);
	File(const FPos& pos, FileSystem* o);
	File(const FPos& pos, const FSBlock& b, FileSystem* o);

};

#endif // FILE_BASE_HPP_INCLUDED