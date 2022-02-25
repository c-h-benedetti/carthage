#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

#include "Folder.hpp"
#include "Versionable.hpp"
#include <memory>

class FileSystem {

	std::unique_ptr<Container> current_obj = nullptr;
	std::vector<FSObject> stack;
	Path current_path;
	const Path user_root;
	const Path vfs_file;

private:

	void open_root();
	void new_vfs(const char* n);
	void make_hierarchy(const Path& out) const;

public:

	void open(const FSObject& obj);

	inline const Path& vfs_path() const{ return this->vfs_file; }
	inline const Path& current_syspath(){ return this->current_path; }
	inline FSType base_class() const{ return this->current_obj->data().flag; }

	// IMPROVE: Must implement previous() and next() methods in FileSystem.
	void next();
	void previous();

	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false, const char* n=nullptr);

	friend class File;
	friend class Folder;
	friend class Versionable;

};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED

/*

Le current est rempli à l'instanciation avec la racine.
Le stack est alors laissé vide, on ne peut appeler previous que si le stack contient qqchose.
En ouvrant le premier élément sans l'ajouter au stack, on le rend impossible à dépiler.

*/