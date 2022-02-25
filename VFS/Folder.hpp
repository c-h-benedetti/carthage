#ifndef VFS_FOLDER_HPP_INCLUDED
#define VFS_FOLDER_HPP_INCLUDED

#include "Container.hpp"

// IMPROVE: [Folder] Make iterators to iterate through one type of flag in the content.

class Folder : public Container{

protected:

	std::vector<FSObject> content;

protected:

	void dispatch(const std::vector<FSObject>& segment) override;

public:

	inline size_t size() const override{ return this->content.size(); }
	inline const FSObject& at(const size_t& i) const override{ return this->content[i]; }

	bool accept_files() const override{ return true; }
	bool accept_folders() const override{ return true; }
	// IMPROVE: [Folder] Modify the code that blocks the creation of a Versionable in a Versionable.
	bool accept_versionables() const override{ return true; }

	void new_file(const ArgsNewFile& args) override;
	void new_folder(const ArgsNewFolder& args) override;
	void new_versionable(const ArgsNewVersionable& fs) override;

	Folder() = delete;
	Folder(FileSystem& fs);
	Folder(const FSObject& obj);
	Folder(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;

};

#endif // VFS_FOLDER_HPP_INCLUDED