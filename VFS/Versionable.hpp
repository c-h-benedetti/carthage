#ifndef VFS_VERSIONABLE_HPP_INCLUDED
#define VFS_VERSIONABLE_HPP_INCLUDED

#include "Folder.hpp"

class Versionable : public Folder{

	Folder* current_version = nullptr; 

private:

	void dispatch(const std::vector<FSObject>& segment) override;

public:

	int commit(const char* msg=nullptr);

	int open() override;

	Versionable() = delete;
	Versionable(const FSObject& obj);
	Versionable(FileSystem& fs, const FSPos& pos, Container* previous=nullptr);
	Versionable(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* previous=nullptr);

	friend class FileSystem;

};

#endif // VFS_VERSIONABLE_HPP_INCLUDED