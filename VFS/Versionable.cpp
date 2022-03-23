#include "Versionable.hpp"
#include "FileSystem.hpp"

// DEBUG: [Versionable] Check that the segment contains exactly one FSType::CURRENT

void Versionable::dispatch(const std::vector<FSObject>& segment){
	this->content.clear();
	this->content.reserve(this->block.nb_folders);
	int found = 0;

	for(const FSObject& obj : segment){
		if (!removed_raised(obj.get_block().flag)){
			if (folder_raised(obj.get_block().flag)){
				if (current_raised(obj.get_block().flag)){
					if (!found){
						this->current_version = new Folder(obj);
						found++;
					}
				}
				else if (version_raised(obj.get_block().flag)){
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


int Versionable::commit(const char* msg){
	
	std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(this->get_block().time));

	FSBlock block = this->current_version->get_block();
	block.name = std::asctime(std::gmtime(&cftime));
	block.flag = pair_flags(FSType::FOLDER, FSType::VERSION);

	this->refer_to.copy_to(
		this->current_version,
		this,
		1,
		&block
	);

	return 0;
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

Versionable::Versionable(const FSObject& obj): Folder(obj){}

Versionable::Versionable(FileSystem& fs, const FSPos& pos, Container* previous): Folder(fs, pos, previous){}

Versionable::Versionable(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* previous): Folder(fs, bck, pos, previous){}