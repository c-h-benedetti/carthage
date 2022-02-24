#ifndef VFS_FOLDER_HPP_INCLUDED
#define VFS_FOLDER_HPP_INCLUDED

#include "File.hpp"
#include "Versionable.hpp"

class Folder : public Container{

	friend class FileSystem;

	std::vector<Folder> folders;
	std::vector<File> files;
	std::vector<Versionable> versionables;

	void dispatch(const std::vector<FSObject>& segment) override;

public:

	inline std::vector<File>& get_files(){ return this->files; }
	inline std::vector<Folder>& get_folders(){ return this->folders; }
	inline std::vector<Versionable>& get_versionables(){ return this->versionables; }

	void open() override;
	FSObject* at(size_t i) override;
	size_t size() const override;

	bool accept_files() const override{return true;}
	void new_file(const ArgsNewFile& args) override;

	bool accept_folders() const override{return true;}
	void new_folder(const ArgsNewFolder& args) override;

	bool accept_versionables() const override{return true;}
	void new_versionable(const ArgsNewVersionable& fs) override;

	Folder() = default;
	Folder(FileSystem* fs);
	Folder(const FSObject& obj);
	Folder(const FSBlock& bck, const FSPos& pos, FileSystem* fs);

};

#endif // VFS_FOLDER_HPP_INCLUDED