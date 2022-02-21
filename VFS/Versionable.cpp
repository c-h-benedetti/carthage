#include "Versionable.hpp"
#include "FileSystem.hpp"

void Versionable::open(){
	this->refer_to->change_directory(std::unique_ptr<Versionable>(new Versionable(*this)));
	this->refer_to->open();
}

void Versionable::dispatch(const std::vector<FSObject>& segment){}

size_t Versionable::size() const{
	return 0;
}

FSObject* Versionable::at(size_t i){
	return nullptr;
}

Versionable::Versionable(FileSystem* fs): Container(fs){}

Versionable::Versionable(const FSObject& obj): Container(obj){}

Versionable::Versionable(const FSBlock& bck, const FSPos& pos, FileSystem* fs): Container(bck, pos, fs){}