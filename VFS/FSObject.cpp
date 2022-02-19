#include "FSObject.hpp"
#include "SystemName.hpp"
#include "FileSystem.hpp"
#include <fstream>


void FSObject::reload_from_vfs(const bool& reset_path){
	std::ifstream str(this->refer_to->vfs_path, std::ios::binary | std::ios::in);
	if (str.is_open()){
		str.seekg(this->block_pos);
		str.read((char*)&this->block, sizeof(FSBlock));
		str.close();
	}
	else{
		std::cerr << "Failed to open VFS" << std::endl;
		return;
	}

	this->block.after_reading();

	// DEBUG: Should raise an exception or a specific behavior
	if (this->block.flag == FSType::REMOVED){
		std::cerr << "The object was removed" << std::endl;
	}

	if (reset_path){
		SystemName sn{this->block.id, this->block.extension};
		this->path = (const char*)&sn;
	}
}


FSObject::FSObject(FileSystem* fs): refer_to(fs){}

FSObject::FSObject(const Path& p, FileSystem* fs): path(p), refer_to(fs){}

FSObject::FSObject(const FSBlock& bck, const FSPos& pos, FileSystem* fs): block_pos(pos), block(bck), refer_to(fs){
	SystemName sn{this->block.id, this->block.extension};
	this->path = (const char*)&sn;
}

FSObject::FSObject(const Path& p, const FSBlock& bck, const FSPos& pos, FileSystem* fs): path(p), block_pos(pos), block(bck), refer_to(fs){

}
