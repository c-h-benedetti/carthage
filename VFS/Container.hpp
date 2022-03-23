#ifndef VFS_CONTAINER_HPP_INCLUDED
#define VFS_CONTAINER_HPP_INCLUDED

#include "FSObject.hpp"
#include <vector>
#include <functional>

class VFSWriter;
class Versionable;

class Container : public FSObject{

protected:

	int create_element(const FSize& count, FSBlock& blc, std::function<void(const Path&, const FSPos&, FSBlock& modif)>& instanciating);
	virtual void dispatch(const std::vector<FSObject>& segment) = 0;
	
public:

	bool name_exists(const Name& n, const Extension& ext) const;
	int load() override;

	inline Versionable& versionable(){ return *((Versionable*)this->previous); }
	inline bool is_versionable() const { 
		if (current_raised(this->block.flag) || version_raised(this->block.flag)){
			return (this->previous && versionable_raised(this->previous->get_block().flag)); 
		}
		return false;
	}

	
	virtual size_t size() const = 0;
	virtual FSObject& at(const size_t& i) = 0;
	virtual const FSObject& read(const size_t& i) const = 0;

	bool accepts(const FSType& flag) const;

	virtual bool accepts_files() const = 0;
	virtual bool accepts_folders() const = 0;
	virtual bool accepts_versionables() const = 0;

	virtual int create_file(const ArgsNewFile& args) = 0;
	virtual int create_folder(const ArgsNewFolder& args) = 0;
	virtual int create_versionable(const ArgsNewVersionable& args) = 0;

	Container() = delete;
	Container(const FSObject& obj);
	Container(FileSystem& fs, const FSPos& pos, Container* previous=nullptr);
	Container(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* previous=nullptr);

	friend class FileSystem;

};


#endif // VFS_CONTAINER_HPP_INCLUDED
