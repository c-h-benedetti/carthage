#include "FileSystem.hpp"
#include <fstream>

Container::Container(FileSystem* fs): FSObject(fs){}

Container::Container(const FSObject& obj): FSObject(obj){}

Container::Container(const FSBlock& bck, const FSPos& pos, FileSystem* fs): FSObject(bck, pos, fs){}


/**
 * Build a system name from a block.
 * Vary if we have a folder or a file (extension)
 */
Path Container::make_path(FSBlock& b) const{
	SystemName sn(b.id, b.extension);
	Path abs_path = this->refer_to->current_syspath() / sn.c_str();

	while (fs::exists(abs_path)){
		b.id.randomize();
		SystemName sn2(b.id, b.extension);
		abs_path = this->refer_to->current_syspath() / sn.c_str();
	}
	return abs_path;
}

const Path& Container::vfs_path() const{
	return this->refer_to->vfs_path();
}

/**
 * Links the last inserted block to the data already present in the VFS.
 * Either by filling the 'content' field, or by browsing the VFS to update the value of 'next'
 */
void Container::linking_segment(const FSPos& insert){
	bool refresh_next = true;
	if (this->block.content == 0){
		this->block.content = insert;
		refresh_next = false;
	}

	if (refresh_next){
		// Fetching the end position of content of this Folder
		FSPos c_pos = this->block.content;
		FSPos last = this->block_pos; // "Content" is the first element of the block, no shift required
		FSize seg_size = 0;

		std::ifstream i_str(this->refer_to->vfs_path(), std::ios::in | std::ios::binary);

		while(c_pos){
			i_str.seekg(c_pos);
			i_str.read((char*)&seg_size, sizeof(FSize));
			last = c_pos + sizeof(FSize) + sizeof(FSBlock) * seg_size;
			i_str.seekg(last);
			i_str.read((char*)&c_pos, sizeof(FSPos));
		}

		i_str.close();

		// Linking the new block to its parent
		std::ofstream l_str(this->refer_to->vfs_path(), std::ios::in | std::ios::out | std::ios::binary);
		l_str.seekp(last);
		l_str.write((char*)&insert, sizeof(FSPos));
		l_str.close();
	}
}

// IMPROVE: [Container] The process to browse the VFS from an object could be refactored in one function. It could use lambda-expressions to do specific tasks.
/**
 * Function browsing the VFS to gather the content of a Container object.
 */
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
	// IMPROVE: [Container] We need to handle the endianness of the [segment's size] and the [next] pointer.
	// IMPROVE: [Container] We definitely need functions to read entities from the VFS and write to it.
	while (c_pos){
		str.seekg(c_pos);
		str.read((char*)&seg_size, sizeof(FSize)); // Will need to handle endianness
		FSBlock blocks[seg_size];
		str.read((char*)blocks, seg_size * sizeof(FSBlock));

		c_pos += sizeof(FSize); // Skip segment size to reach content of first block. 
		for (size_t i = 0 ; i < seg_size ; i++){
			blocks[i].after_reading();
			if (!removed_raised(blocks[i].flag)){
				segment.push_back(FSObject(blocks[i], c_pos, this->refer_to));
			}
			c_pos += sizeof(FSBlock);
		}
		// DEBUG: Add assert that checks if the reserved size matches the final size of the vector
		str.read((char*)&c_pos, sizeof(FSPos)); // Will need to handle endianness
	}
}

/**
 * Function loading the content of a Container object.
 */
void Container::load(){
	std::vector<FSObject> content;
	this->collect_content(content);
	this->dispatch(content);
}