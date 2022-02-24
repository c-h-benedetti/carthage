#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

#include "Folder.hpp"
#include "Versionable.hpp"
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

	inline const Path& current_syspath(){ return this->current_path; }

	inline FSType base_class() const{ return this->current_obj->data().flag; }

	inline Folder* get_folder() { 
		if ((this->base_class() == FSType::FOLDER) || (this->base_class() == FSType::ROOT)){
			return (Folder*)this->current_obj.get();
		}
		else{
			return nullptr;
		}
	}

	inline Versionable* get_versionable() { 
		if (this->base_class() == FSType::VERSIONABLE){
			return (Versionable*)this->current_obj.get();
		}
		else{
			return nullptr;
		}
	}

	// IMPROVE: Must implement previous() and next() methods in FileSystem.
	void next();
	void previous();
	void make_hierarchy(const Path& out) const;

};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED

/*

Le current est rempli à l'instanciation avec la racine.
Le stack est alors laissé vide, on ne peut appeler previous que si le stack contient qqchose.
En ouvrant le premier élément sans l'ajouter au stack, on le rend impossible à dépiler.

*/