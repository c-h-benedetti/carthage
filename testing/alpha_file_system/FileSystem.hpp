#ifndef FILE_SYSTEM_HPP_INCLUDED
#define FILE_SYSTEM_HPP_INCLUDED

#include "Folder.hpp"

class FileSystem : public Folder{

	FileSystem(const FileSystem&) = delete;
	FileSystem& operator=(const FileSystem&) = delete;
	FileSystem(FileSystem&&) = delete;
	FileSystem& operator=(FileSystem&&) = delete;

	FileSystem();
	FileSystem(const std::string& s);

	~FileSystem();

	static FileSystem* ptr_file_system;

	void detect_file_system();
	void save_state() const;

public:
	
	static FileSystem& file_system();
	static FileSystem& file_system(const std::string& s);

	static void terminate();

};

#endif //FILE_SYSTEM_HPP_INCLUDED


/*

[ ] Replace the call to terminate by the usage of a unique_ptr

*/