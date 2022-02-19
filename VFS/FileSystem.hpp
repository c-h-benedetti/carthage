#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

#include "Container.hpp"
#include <memory>

class FileSystem {

	std::unique_ptr<Container> current_obj = nullptr;
	Path current_path;
	std::vector<Container> stack;
	const Path user_root;
	const Path vfs_file;

private:

	void new_vfs();

public:

	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false);

	inline const Path& vfs_path() const{ return this->vfs_file; }
	void change_directory(std::unique_ptr<Container>&& c);
	void open_root();

	void next();
	void previous();

};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED

/*

Le current est rempli à l'instanciation avec la racine.
Le stack est alors laissé vide, on ne peut appeler previous que si le stack contient qqchose

*/