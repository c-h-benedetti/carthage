#ifndef FILE_SYSTEM_OBJECT_HPP_INCLUDED
#define FILE_SYSTEM_OBJECT_HPP_INCLUDED

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class FileSystemObject{

protected:

	fs::path path;
	FileSystemObject* parent = nullptr;

	void from_root_rec(fs::path& p) const;

public:

	bool exists() const{ return fs::exists(this->path); }

	FileSystemObject();
	FileSystemObject(const std::string& s);

	fs::path from_root() const;
	std::string name() const;

};

#endif //FILE_SYSTEM_OBJECT_HPP_INCLUDED