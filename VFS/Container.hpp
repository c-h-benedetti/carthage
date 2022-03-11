#ifndef VFS_CONTAINER_HPP_INCLUDED
#define VFS_CONTAINER_HPP_INCLUDED

#include "FSObject.hpp"
#include <vector>
#include <functional>


class Container : public FSObject{

protected:

	int create_element(const FSize& count, FSBlock& blc, std::function<void(const Path&, const FSPos&)>& instanciating);
	virtual void dispatch(const std::vector<FSObject>& segment) = 0;
	int link_segment(const FSPos& insert);
	
public:

	bool name_exists(const Name& n, const Extension& ext) const;
	int load() override;

	
	virtual size_t size() const = 0;
	virtual const FSObject& at(const size_t& i) const = 0;

	virtual bool accepts_files() const = 0;
	virtual bool accepts_folders() const = 0;
	virtual bool accepts_versionables() const = 0;

	virtual int create_file(const ArgsNewFile& args) = 0;
	virtual int create_folder(const ArgsNewFolder& args) = 0;
	virtual int create_versionable(const ArgsNewVersionable& args) = 0;

	Container() = delete;
	Container(FileSystem& fs);
	Container(const FSObject& obj);
	Container(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;

};


#endif // VFS_CONTAINER_HPP_INCLUDED