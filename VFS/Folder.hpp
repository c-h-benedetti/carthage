#ifndef VFS_FOLDER_HPP_INCLUDED
#define VFS_FOLDER_HPP_INCLUDED

#include "Container.hpp"

class File;

class Folder : public Container{

protected:

	std::vector<FSObject> content;

protected:

	void dispatch(const std::vector<FSObject>& segment) override;

public:

	int open() override;

	inline size_t size() const override{ return this->content.size(); }
	inline FSObject& at(const size_t& i) override{ return this->content[i]; }
	inline const FSObject& read(const size_t& i) const override{ return this->content[i]; }

	bool accepts_files() const override{ return true; }
	bool accepts_folders() const override{ return true; }
	bool accepts_versionables() const override;

	int create_file(const ArgsNewFile& args) override;
	int create_folder(const ArgsNewFolder& args) override;
	int create_versionable(const ArgsNewVersionable& fs) override;

	Folder() = delete;
	Folder(const FSObject& obj);
	Folder(FileSystem& fs, const FSPos& pos, Container* previous=nullptr);
	Folder(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* previous=nullptr);

	friend class FileSystem;

};

#endif // VFS_FOLDER_HPP_INCLUDED
