#ifndef VFS_CONTAINER_HPP_INCLUDED
#define VFS_CONTAINER_HPP_INCLUDED

#include "FSObject.hpp"
#include <vector>
#include <functional>

class Container : public FSObject{

	friend class FileSystem;

protected:

	virtual void dispatch(const std::vector<FSObject>& segment) = 0;
	void collect_content(std::vector<FSObject>& segment);
	void load() override;
	Path make_path(FSBlock& b) const;
	void linking_segment(const FSPos& insert);
	const Path& vfs_path() const;

	template <size_t T>
	FSPos blank_segments() const;

	template <typename T, size_t N>
	void create_element(const Name& n, const Extension& ext, FSBlock& blc, std::function<T(const Path&, const FSPos&)>& instanciating, std::vector<T>& collection);

public:

	virtual FSObject* at(size_t i) = 0;
	inline FSObject* operator[](size_t i){ return this->at(i); }

	virtual size_t size() const = 0;

	virtual bool accept_files() const{return false;}
	virtual void new_file(const ArgsNewFile& fs){};

	virtual bool accept_folders() const{return false;}
	virtual void new_folder(const ArgsNewFolder& fs){};

	virtual bool accept_versionables() const{return false;}
	virtual void new_versionable(const ArgsNewVersionable& fs){};

	Container() = default;
	Container(FileSystem* fs);
	Container(const FSObject& obj);
	Container(const FSBlock& bck, const FSPos& pos, FileSystem* fs);

};

template <typename T>
bool name_exists(const Name& n, const Extension& ext, const std::vector<T>& objs);

#include "Container.tpp"

#endif // VFS_CONTAINER_HPP_INCLUDED