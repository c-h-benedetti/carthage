#ifndef VFS_CONTAINER_HPP_INCLUDED
#define VFS_CONTAINER_HPP_INCLUDED

#include "FSObject.hpp"
#include <vector>

class Container : public FSObject{

	friend class FileSystem;

protected:

	virtual void dispatch(const std::vector<FSObject>& segment) = 0;
	void collect_content(std::vector<FSObject>& segment);
	void load() override;

public:

	virtual FSObject* at(size_t i) = 0;
	inline FSObject* operator[](size_t i){ return this->at(i); }

	virtual size_t size() const = 0;

	Container() = default;
	Container(FileSystem* fs);
	Container(const FSObject& obj);
	Container(const FSBlock& bck, const FSPos& pos, FileSystem* fs);

};

#endif // VFS_CONTAINER_HPP_INCLUDED