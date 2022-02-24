#include "Versionable.hpp"
#include "FileSystem.hpp"
// #include "Folder.hpp"

void Versionable::open(){
	this->refer_to->change_directory(std::unique_ptr<Versionable>(new Versionable(*this)));
	this->refer_to->open();
}

void Versionable::dispatch(const std::vector<FSObject>& segment){
	// this->older_versions.clear();
	// this->older_versions.reserve(this->block.nb_folders); // nb_folders == nb_versions - 1

	// // DEBUG: [Versionable] Check that the segment contains exactly one FSType::CURRENT
	// for(const FSObject& fobj : segment){
	// 	if (folder_raised(fobj.data().flag)){
	// 		if (current_raised(fobj.data().flag)){
	// 			this->current_version = std::unique_ptr<Folder>(new Folder(fobj));
	// 		}
	// 		else if (version_raised(fobj.data().flag)){
	// 			this->older_versions.push_back(Folder(fobj));
	// 		}
	// 	}
	// }
}

Versionable::~Versionable(){
	delete this->current_version;
	this->current_version = nullptr;
}

void Versionable::load(){
	// this->Container::load();
	// this->current_version->load();
}

size_t Versionable::size() const{
	return 0;
}

FSObject* Versionable::at(size_t i){
	return nullptr;
}

Versionable::Versionable(FileSystem* fs): Container(fs){}

Versionable::Versionable(const FSObject& obj): Container(obj){}

Versionable::Versionable(const FSBlock& bck, const FSPos& pos, FileSystem* fs): Container(bck, pos, fs){}