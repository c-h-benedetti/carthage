#ifndef VFS_VERSIONABLE_HPP_INCLUDED
#define VFS_VERSIONABLE_HPP_INCLUDED

#include "Container.hpp"

class Folder;

// Cannot inherit from Folder because of cross inclusion.
//  => We cannot both include Folder.hpp here AND have a vector of Versionable in Folder.

class Versionable : public Container{

	friend class FileSystem;

	Folder* current_version = nullptr;
	// // IMPROVE: [Versionable] Should be Folder, but std::vector needs the full declaration of the class. Will change when using custom maps instead of vectors.
	std::vector<FSObject> older_versions; 

	void dispatch(const std::vector<FSObject>& segment) override;
	void load() override;

public:

	void open() override;
	size_t size() const override;
	FSObject* at(size_t i);

	~Versionable();

	Versionable() = default;
	Versionable(FileSystem* fs);
	Versionable(const FSObject& obj);
	Versionable(const FSBlock& bck, const FSPos& pos, FileSystem* fs);

};

#endif // VFS_VERSIONABLE_HPP_INCLUDED