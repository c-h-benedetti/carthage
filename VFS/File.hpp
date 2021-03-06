#ifndef VFS_FILE_HPP_INCLUDED
#define VFS_FILE_HPP_INCLUDED

#include "FSObject.hpp"

class File : public FSObject{

	int load() override;

public:

	int open() override;

	File() = delete;
	File(FileSystem& fs);
	File(const FSObject& obj);
	File(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;
	
};

#endif // VFS_FILE_HPP_INCLUDED