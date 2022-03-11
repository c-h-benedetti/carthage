#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

#include "Folder.hpp"
#include "Versionable.hpp"
#include <memory>

class FileSystem {

	std::unique_ptr<Container> current_obj = nullptr;
	std::vector<FSObject> stack;
	Path current_path;
	size_t head = 0;
	const Path user_root;
	const Path vfs_file;

private:

	void open_root();
	void new_vfs(const char* n);

public:

	void open(const FSObject& obj);

	inline const Path& vfs_path() const{ return this->vfs_file; }
	inline const Path& current_syspath(){ return this->current_path; }
	inline FSType base_class() const{ return this->current_obj->data().flag; }
	void make_hierarchy(const Path& out) const;

	// IMPROVE: Must implement previous() and next() methods in FileSystem.
	void next();
	void previous();

	inline std::unique_ptr<Container>& current(){ return this->current_obj; }
	inline Folder* folder() { return (folder_raised(this->current_obj->data().flag)) ? ((Folder*)this->current_obj.get()) : (nullptr); }
	inline Versionable* versionable() { return (versionable_raised(this->current_obj->data().flag)) ? ((Versionable*)this->current_obj.get()) : (nullptr); }

	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false, const char* n=nullptr);

	friend class File;
	friend class Folder;
	friend class Versionable;

};

class FileSystem {

	/// Contains the Containers from the root of the project to the current one.
	std::vector<Container*> stack;
	/// Root of the project.
	const Path user_root;

public:

	/// Path to the file representing the VFS.
	const Path vfs_path; 

private:

	/// Opens the root of the project from the VFS.
	int open_root(); 
	/// Creates a new Carthage setup from the user provided path. 'n' is the potential name of the project, but can be nullptr.
	int new_vfs(const char* n); 

public:

	/// Opens the FSObject passed in argument. Sets it as current object if it is a Container.
	void open(const FSObject& obj);
	/// Returns the current object (last item of the 'stack' vector)
	const Container* current() const;
	/// Returns the current object (last item of the 'stack' vector) casted as a Versionable
	const Versionable* current_as_versionable() const;
	/// Builds the current path from the elements contained in the 'stack'
	const Path current_path() const;
	/// Creates an HTML file (out is a path: "/abc/def/.../xxx.html")
	void make_hierarchy(const Path& out) const;


	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false, const char* n=nullptr);

};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED

/*

Le current est rempli à l'instanciation avec la racine.
Le stack est alors laissé vide, on ne peut appeler previous que si le stack contient qqchose.
En ouvrant le premier élément sans l'ajouter au stack, on le rend impossible à dépiler.

*/