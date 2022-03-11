#include "Versionable.hpp"
#include "FileSystem.hpp"

// DEBUG: [Versionable] Check that the segment contains exactly one FSType::CURRENT

void Versionable::dispatch(const std::vector<FSObject>& segment){
	this->content.clear();
	this->content.reserve(this->block.nb_folders);
	int found = 0;

	for(const FSObject& obj : segment){
		if (!removed_raised(obj.get_data().flag)){
			if (!folder_raised(obj.get_data().flag)){
				if (current_raised(obj.get_data().flag)){
					if (!found){
						this->current_version = new Folder(obj);
						found++;
					}
				}
				else if (version_raised(obj.get_data().flag)){
					this->content.push_back(obj);
				}
			}
		}
	}
}


int Versionable::open(){
	this->load();

	if (this->current_version){
		this->current_version->open();
		this->refer_to.add(this->current_version);
		return 0;
	}
	else{
		return 1;
	}
}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Versionable::Versionable(FileSystem& fs): Folder(fs){}

Versionable::Versionable(const FSObject& obj): Folder(obj){}

Versionable::Versionable(FileSystem& fs, const FSBlock& bck, const FSPos& pos): Folder(fs, bck, pos){}