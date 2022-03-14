#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

// IMPROVE: [FileSystem] The VFS_IO must not be owned by the FileSystem.

#include "VFS_IO.hpp"
#include "Versionable.hpp"
#include <memory>


class FileSystem {

	// IMPROVE: [FileSystem] The VFS_IO object is supposed to be a reference acquired from the pProject.
	VFS_IO io_manager;
	std::vector<Container*> stack;

private:

	int open_root(); /// Opens the root of the project from the VFS.
	int new_vfs(const char* n); /// Creates a new Carthage setup from the user provided path.
	inline void add(Container* c){ if (c){this->stack.push_back(c);} }
	inline int open(){ return this->stack.back()->load(); }
	void create(const FSBlock& src_block, const Path& src_path, const Path& dst_path);

public:

	inline VFS_IO& vfs_io() { return this->io_manager; }
	inline Container* current() { return this->stack.back(); }

	void replicate_to(const FSObject& source, Container* dest, const bool& deduce_src, const bool& deduce_dst);

	//void make_hierarchy(const Path& out) const; /// Creates an HTML file (out is a path: "/abc/def/.../xxx.html")

	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false, const char* n=nullptr);

	friend class FSObject;
	friend class Versionable;
};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED

/*

Le current est rempli à l'instanciation avec la racine.
Le stack est alors laissé vide, on ne peut appeler previous que si le stack contient qqchose.
En ouvrant le premier élément sans l'ajouter au stack, on le rend impossible à dépiler.

*/