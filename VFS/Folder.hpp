#ifndef VFS_FOLDER_HPP_INCLUDED
#define VFS_FOLDER_HPP_INCLUDED

#include "Container.hpp"
#include "File.hpp"
#include "Versionable.hpp"

class Folder : public Container{

	friend class FileSystem;

	std::vector<Folder> folders;
	std::vector<File> files;
	std::vector<Versionable> versionables;

	void dispatch(const std::vector<FSObject>& segment) override;

public:

	void open() override;
	FSObject* at(size_t i) override;
	size_t size() const override;

	Folder() = default;
	Folder(FileSystem* fs);
	Folder(const FSObject& obj);
	Folder(const FSBlock& bck, const FSPos& pos, FileSystem* fs);

};

#endif // VFS_FOLDER_HPP_INCLUDED