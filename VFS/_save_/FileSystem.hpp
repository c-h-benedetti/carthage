#ifndef VFS_FILE_SYSTEM_HPP_INCLUDED
#define VFS_FILE_SYSTEM_HPP_INCLUDED

#include "Folder.hpp"
#include <memory>

class FileSystem : public Container{

	Path full_path;
	Path root;
	std::unique_ptr<Container> current = nullptr;
	std::vector<Container> stack;
	std::vector<Folder> folders;

private:

	void new_vfs();
	void dispatch(const std::vector<FSObject>& segment) override;

public:

	const Path vfs_path;

	FileSystem() = delete;
	FileSystem(const Path& p, bool reset=false);

	size_t size() const override;
	FSObject* at(size_t i) override;
	void change_directory(std::unique_ptr<Container>&& c);

	void open() override;
	void next();
	void previous();

};

#endif // VFS_FILE_SYSTEM_HPP_INCLUDED