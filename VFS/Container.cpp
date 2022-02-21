#include "FileSystem.hpp"
#include <fstream>

Container::Container(FileSystem* fs): FSObject(fs){}

Container::Container(const FSObject& obj): FSObject(obj){}

Container::Container(const FSBlock& bck, const FSPos& pos, FileSystem* fs): FSObject(bck, pos, fs){}

void Container::collect_content(std::vector<FSObject>& segment){
	FSPos c_pos = this->block.content;
	FSize seg_size = 0;
	segment.clear();
	segment.reserve(this->block.nb_files + this->block.nb_folders + this->block.nb_versionables);

	std::ifstream str(this->refer_to->vfs_path(), std::ios::binary | std::ios::in);

	if (!str.is_open()){
		std::cerr << "Failed to open VFS" << std::endl;
		return;
	}

	while (c_pos){
		str.seekg(c_pos);
		str.read((char*)&seg_size, sizeof(FSize)); // Will need to handle endianness
		FSBlock blocks[seg_size];
		str.read((char*)blocks, seg_size * sizeof(FSBlock));

		for (size_t i = 0 ; i < seg_size ; i++){
			blocks[i].after_reading();
			if (blocks[i].flag != FSType::REMOVED){
				segment.push_back(FSObject(blocks[i], c_pos, this->refer_to));
			}
			c_pos += sizeof(FSBlock);
		}
		// DEBUG: Add assert that checks if the reserved size matches the final size of the vector
		str.read((char*)&c_pos, sizeof(FSPos)); // Will need to handle endianness
	}
}

void Container::load(){
	std::vector<FSObject> content;
	this->collect_content(content);
	this->dispatch(content);
}