#ifndef VFS_FOLDER_HPP_INCLUDED
#define VFS_FOLDER_HPP_INCLUDED

#include "Container.hpp"

class File;
class Versionable;


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
	inline Versionable& versionable(){ return *((Versionable*)this->previous); }
	inline bool is_versionable() const { 
		if (current_raised(this->block.flag) || version_raised(this->block.flag)){
			return (this->previous && versionable_raised(this->previous->get_data().flag)); 
		}
		return false;
	}

	bool accepts_files() const override{ return true; }
	bool accepts_folders() const override{ return true; }
	bool accepts_versionables() const override;

	int create_file(const ArgsNewFile& args) override;
	int create_folder(const ArgsNewFolder& args) override;
	int create_versionable(const ArgsNewVersionable& fs) override;

	Folder() = delete;
	Folder(FileSystem& fs);
	Folder(const FSObject& obj);
	Folder(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;

};

#endif // VFS_FOLDER_HPP_INCLUDED
