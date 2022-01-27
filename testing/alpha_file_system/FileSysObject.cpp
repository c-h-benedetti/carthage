#include "FileSysObject.hpp"

FileSystemObject::FileSystemObject(){

}

FileSystemObject::FileSystemObject(const std::string& s){
	this->path = s.c_str();
}

fs::path FileSystemObject::from_root() const{
	fs::path p1{};
	this->from_root_rec(p1);
	return p1;
}

void FileSystemObject::from_root_rec(fs::path& p) const{
	p = this->path / p;
	if (this->parent != nullptr){
		this->parent->from_root_rec(p);
	}
}

std::string FileSystemObject::name() const{
	return this->path;
}