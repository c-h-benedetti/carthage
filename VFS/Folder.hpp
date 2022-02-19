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
	Folder(const Path& p, FileSystem* fs);
	Folder(const FSBlock& bck, const FSPos& pos, FileSystem* fs);
	Folder(const FSObject& obj);

	/*void add_file(const SystemName& name, FileSystem* fs);
	void add_folder(const SystemName& name, FileSystem* fs);
	void add_files(const std::vector<SystemName>& names, FileSystem* fs);
	void add_folders(const std::vector<SystemName>& names, FileSystem* fs);*/

};

#endif // VFS_FOLDER_HPP_INCLUDED