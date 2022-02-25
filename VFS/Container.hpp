#ifndef VFS_CONTAINER_HPP_INCLUDED
#define VFS_CONTAINER_HPP_INCLUDED

#include "FSObject.hpp"
#include <vector>
#include <functional>

class Container : public FSObject{

protected:

	template <size_t T>
	FSPos blank_segments() const;

	template <size_t N>
	int create_element(FSBlock& blc, std::function<FSObject(const Path&, const FSPos&)>& instanciating,	std::vector<FSObject>& collection);

	void load() override;
	Path make_path(FSBlock& b) const;
	int linking_segment(const FSPos& insert);
	bool name_exists(const Name& n, const Extension& ext);
	int collect_content(std::vector<FSObject>& segment);
	virtual void dispatch(const std::vector<FSObject>& segment) = 0;
	const Path& vfs_path() const;
	
public:

	virtual size_t size() const = 0;
	virtual const FSObject& at(const size_t& i) const = 0;
	inline const FSObject& operator[](const size_t& i){ return this->at(i); }

	virtual bool accept_files() const = 0;
	virtual bool accept_folders() const = 0;
	virtual bool accept_versionables() const = 0;

	virtual void new_file(const ArgsNewFile& fs) = 0;
	virtual void new_folder(const ArgsNewFolder& fs) = 0;
	virtual void new_versionable(const ArgsNewVersionable& fs) = 0;

	Container() = delete;
	Container(FileSystem& fs);
	Container(const FSObject& obj);
	Container(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;

};


#include "Container.tpp"

#endif // VFS_CONTAINER_HPP_INCLUDED