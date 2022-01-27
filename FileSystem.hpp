#ifndef FILE_SYSTEM_BASE_HPP_INCLUDED
#define FILE_SYSTEM_BASE_HPP_INCLUDED

#include "Folder.hpp"

class FileSystem : public Folder{

	fs::path base_path;
	fs::path data_root;
	fs::path descriptor;

	friend class File;
	friend class Folder;
	friend class Versionable;
	friend class FSObject;

public:

	FileSystem();
	FileSystem(const fs::path& p, const Name& n);

	File get_file(const FPos& p);
	Folder get_folder(const FPos& p);
	Versionable get_versionable(const FPos& p);

	File get_file(const ID& id);
	Folder get_folder(const ID& id);
	Versionable get_versionable(const ID& id);

};

#endif // FILE_SYSTEM_BASE_HPP_INCLUDED