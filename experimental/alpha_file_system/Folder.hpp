#ifndef FOLDER_HPP_INCLUDED
#define FOLDER_HPP_INCLUDED

#include <vector>
#include "FileSysObject.hpp"
#include "File.hpp"

class Folder : public FileSystemObject{

protected:

	friend class FileSystem;

	std::vector<Folder> _folders;
	std::vector<File>   _files;

	void detect_file_system_rec(const fs::directory_entry& d);

public:

	Folder();
	Folder(const std::string& s);
	Folder(const std::string& s, const fs::directory_entry& d);

	const std::vector<Folder>& folders() const{ return this->_folders; }
	const std::vector<File>& files() const{ return this->_files; }

};


#endif //FOLDER_HPP_INCLUDED