#include "FSObject.hpp"
#include "SystemName.hpp"
#include "FileSystem.hpp"
#include <fstream>

/**
 * Refresh the block in the VFS file from the FSBlock stored in this instance.
 * Only this block is affected, it is not a general refreshing of the VFS.
 * The position where to write is known through FSObject::block_pos
 */
void FSObject::refresh_vfs() const{
	// Making a version able to be writen in the VFS and putting it inside a buffer.
	FSBlock b = this->block;
	b.before_writting();
	BasicBuffer<sizeof(FSBlock)> buffer;
	buffer.add<FSBlock>(b);

	// Moving to the position of this object in the VFS and overwriting the block with the content of the buffer.
	std::ofstream str(this->refer_to->vfs_path(), std::ios::out | std::ios::in | std::ios::binary);
	if (str.is_open()){
		str.seekp(this->block_pos);
		buffer.write_to(str);
		str.close();
	}
	// IMPROVE: [FSObject] Add an exception if the VFS can't be opened in refresh_vfs().
}


/**
 * Refresh the FSBlock: FSObject::block from the VFS.
 * We do not browse the whole VFS to reach the correct block, we use the FSObject::block_pos to fetch only the required data.
 */
void FSObject::reload_from_vfs(){
	std::ifstream str(this->refer_to->vfs_path(), std::ios::binary | std::ios::in);
	if (str.is_open()){
		str.seekg(this->block_pos);
		str.read((char*)&this->block, sizeof(FSBlock));
		str.close();
	}
	else{
		// IMPROVE: [FSObject] Need better errors handling in reload_from_vfs().
		// IMPROVE: [FSObject] Encapsulate the procedures to generate VFS streams in functions handling errors?
		// IMPROVE: [FSObject] Instead of sending everything to cerr, we should use a global stream for logs.
		std::cerr << "Failed to open VFS" << std::endl;
		return;
	}

	this->block.after_reading();

	// IMROVE: [FSObject] If the current object was removed, it should raise an exception.
	if (file_raised(this->block.flag)){
		std::cerr << "The object was removed" << std::endl;
		return;
	}

	SystemName sn{this->block.id, this->block.extension};
	this->path = sn.c_str();
}

// IMPROVE: [FSObject] At least check that we don't have a nullptr.
FSObject::FSObject(FileSystem* fs): refer_to(fs){}

// IMPROVE: [FSObject] Need refactoring.
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
