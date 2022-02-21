#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

#include "Container.hpp"
#include <memory>

class FileSystem {

	friend class File;
	friend class Folder;
	friend class Versionable;

	std::unique_ptr<Container> current_obj = nullptr;
	Path current_path;
	std::vector<FSObject> stack;
	const Path user_root;
	const Path vfs_file;

private:

	void open();
	void open_root();
	void new_vfs(const char* n);
	void change_directory(std::unique_ptr<Container>&& c);

public:

	// IMPROVE: Manage operators overloading, copying, ...
	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false, const char* n=nullptr);

	inline const Path& vfs_path() const{ return this->vfs_file; }
	inline const std::unique_ptr<Container>& current(){ return this->current_obj; }

	void next();
	void previous();

};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED

/*

Le current est rempli à l'instanciation avec la racine.
Le stack est alors laissé vide, on ne peut appeler previous que si le stack contient qqchose.
En ouvrant le premier élément sans l'ajouter au stack, on le rend impossible à dépiler.

*/