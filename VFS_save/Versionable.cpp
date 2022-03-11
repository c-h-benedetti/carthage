#include "Versionable.hpp"

// DEBUG: [Versionable] Check that the segment contains exactly one FSType::CURRENT

void Versionable::dispatch(const std::vector<FSObject>& segment){
	this->content.clear();
	this->content.reserve(this->block.nb_files + this->block.nb_folders + this->block.nb_versionables);

	for(const FSObject& obj : segment){
		if (!removed_raised(obj.data().flag)){
			if (!folder_raised(obj.data().flag)){
				if (current_raised(obj.data().flag)){
					this->current_version = std::unique_ptr<Folder>(new Folder(obj));
				}
				else if (version_raised(obj.data().flag)){
					this->content.push_back(obj);
				}
			}
		}
	}
}


void Versionable::load(){
	this->Folder::load();
	this->current_version->load();
}


void Versionable::join(Path& p){
	p = p / this->system_name / this->current_version->get_system_name();
}


void Versionable::unjoin(Path& p){
	p = p.parent_path().parent_path();
}

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Versionable::Versionable(FileSystem& fs): Folder(fs){}

Versionable::Versionable(const FSObject& obj): Folder(obj){}

Versionable::Versionable(FileSystem& fs, const FSBlock& bck, const FSPos& pos): Folder(fs, bck, pos){}