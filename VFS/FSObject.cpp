#include "FSObject.hpp"
#include "SystemName.hpp"
#include "FileSystem.hpp"
#include <fstream>


void FSObject::reload_from_vfs(){
	std::ifstream str(this->refer_to->vfs_path(), std::ios::binary | std::ios::in);
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

	// IMROVE: Should raise an exception or a specific behavior
	if (this->block.flag == FSType::REMOVED){
		std::cerr << "The object was removed" << std::endl;
	}

	if (this->block.extension.length()){
		SystemName sn{this->block.id, this->block.extension};
		this->path = sn.c_str();
	}
	else{
		SystemName sn{this->block.id};
		this->path = sn.c_str();
	}
}


FSObject::FSObject(FileSystem* fs): refer_to(fs){}

FSObject::FSObject(const FSBlock& bck, const FSPos& pos, FileSystem* fs): block_pos(pos), block(bck), refer_to(fs){
	if (this->block.extension.length()){
		SystemName sn{this->block.id, this->block.extension};
		this->path = sn.c_str();
	}
	else{
		SystemName sn{this->block.id};
		this->path = sn.c_str();
	}
}
