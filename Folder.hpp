#ifndef FOLDER_BASE_HPP_INCLUDED
#define FOLDER_BASE_HPP_INCLUDED

#include <vector>
#include "File.hpp"

class FileSystem;
class Versionable;

class Folder : public FSObject{

	File* files = nullptr;
	Folder* folders = nullptr;
	Versionable* versionables = nullptr;

public:

	Folder(FileSystem* o);
	Folder(const FPos& pos, FileSystem* o);
	Folder(const FPos& pos, const FSBlock& b, FileSystem* o);

	void load() override;

	virtual ~Folder();

};

#endif // FOLDER_BASE_HPP_INCLUDED