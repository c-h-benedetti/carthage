#ifndef FS_OBJECT_HPP_INCLUDED
#define FS_OBJECT_HPP_INCLUDED

#include "FSBlock.hpp"

class FileSystem;

class FSObject{

protected:

	FPos fpos = 0;
	FSBlock block;
	FileSystem* owner = nullptr;

	FSObject();
	FSObject(FileSystem* o);
	FSObject(const FPos& pos, FileSystem* o);
	FSObject(const FPos& pos, const FSBlock& b, FileSystem* o);

	virtual void load();

public:

	fs::path build_path() const;

};

#endif // FS_OBJECT_HPP_INCLUDED