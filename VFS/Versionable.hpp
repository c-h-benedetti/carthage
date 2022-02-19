#ifndef VFS_VERSIONABLE_HPP_INCLUDED
#define VFS_VERSIONABLE_HPP_INCLUDED

#include "Container.hpp"

class Versionable : public Container{

	friend class FileSystem;

	void dispatch(const std::vector<FSObject>& segment) override;

public:

	void open() override;
	size_t size() const override;
	FSObject* at(size_t i);

	Versionable() = default;
	Versionable(const Path& p, FileSystem* fs);
	Versionable(const FSBlock& bck, const FSPos& pos, FileSystem* fs);
	Versionable(const FSObject& obj);

};

#endif // VFS_VERSIONABLE_HPP_INCLUDED