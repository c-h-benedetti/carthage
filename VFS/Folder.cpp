#include "Folder.hpp"
#include "FileSystem.hpp"
#include <fstream>

void Folder::open(){
	this->refer_to->change_directory(std::unique_ptr<Folder>(new Folder(*this)));
	this->refer_to->open();
}


FSObject* Folder::at(size_t i){
	if (i < folders.size()){
		return &(this->folders[i]);
	}
	i -= folders.size();
	if (i < files.size()){
		return &(this->files[i]);
	}
	i -= files.size();
	return &(this->versionables[i]);
}


size_t Folder::size() const{
	return this->folders.size() + this->files.size() + this->versionables.size();
}


void Folder::dispatch(const std::vector<FSObject>& segment){
	this->folders.clear();
	this->folders.reserve(this->block.nb_folders);

	this->files.clear();
	this->files.reserve(this->block.nb_files);

	this->versionables.clear();
	this->versionables.reserve(this->block.nb_versionables);


	for(size_t i = 0 ; i < segment.size() ; i++){
		// IMPROVE: Use FSType methods instead of a switch
		switch (segment[i].data().flag){
			case(FSType::FILE):
				this->files.push_back(File(segment[i]));
				break;
			case(FSType::FOLDER):
				this->folders.push_back(Folder(segment[i]));
				break;
			case(FSType::VERSIONABLE):
				this->versionables.push_back(Versionable(segment[i]));
				break;
		}
	}
}


Folder::Folder(FileSystem* fs): Container(fs){}

Folder::Folder(const FSObject& obj): Container(obj){}

Folder::Folder(const FSBlock& bck, const FSPos& pos, FileSystem* fs): Container(bck, pos, fs){}
