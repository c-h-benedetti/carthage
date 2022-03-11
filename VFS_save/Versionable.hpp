#ifndef VFS_VERSIONABLE_HPP_INCLUDED
#define VFS_VERSIONABLE_HPP_INCLUDED

#include "Folder.hpp"

class Versionable : public Folder{

	std::unique_ptr<Folder> current_version; 

private:

	void dispatch(const std::vector<FSObject>& segment) override;
	void load() override;

public:

	void join(Path& p) override;
	void unjoin(Path& p) override;
	Path data_path() const override { return this->system_name / this->current_version->get_system_name(); }

	// IMPROVE: [Versionable] Override Folder methods or transmit their call to current_version?

	Versionable() = delete;
	Versionable(FileSystem& fs);
	Versionable(const FSObject& obj);
	Versionable(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;

};

#endif // VFS_VERSIONABLE_HPP_INCLUDED