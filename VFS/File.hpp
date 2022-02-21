#ifndef VFS_FILE_HPP_INCLUDED
#define VFS_FILE_HPP_INCLUDED

#include "FSObject.hpp"

class File : public FSObject{

	friend class FileSystem;

	void load() override;

public:

	void open() override;

	File() = default;
	File(FileSystem* fs);
	File(const FSObject& obj);
	File(const FSBlock& bck, const FSPos& pos, FileSystem* fs);
};

#endif // VFS_FILE_HPP_INCLUDED