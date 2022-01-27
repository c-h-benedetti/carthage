#include "FileSystem.hpp"
#include "Versionable.hpp"

FileSystem::FileSystem(const fs::path& p, const Name& n): Folder(0, this), base_path(p){
	char buffer[sizeof(Name)+1] = {};
	n.valid(buffer);
	this->data_root = this->base_path / buffer;
	this->descriptor = this->base_path / CARTHAGE_DIR / DESCRIPTOR;
}

FileSystem::FileSystem(): Folder(0, this){}

File FileSystem::get_file(const FPos& p){
	return File{p, this};
}

Folder FileSystem::get_folder(const FPos& p){
	return Folder{p, this};
}

Versionable FileSystem::get_versionable(const FPos& p){
	return Versionable{p, this};
}

File FileSystem::get_file(const ID& id){
	return File{this};
}

Folder FileSystem::get_folder(const ID& id){
	return Folder{this};
}

Versionable FileSystem::get_versionable(const ID& id){
	return Versionable{this};
}