#include "FSObject.hpp"
#include "SystemName.hpp"
#include "FileSystem.hpp"
#include <fstream>

/**
 * Overrides the block in the VFS file from this instance's FSObject::block (FSBlock).
 * Only this block is affected, it is not a general refreshing of the VFS.
 * The position where to write is known through FSObject::block_pos.
 * 
 * | Returns 0 on success.
 * | Returns 1 if the VFS could not be opened.
 */
int FSObject::override_vfs() const{
	// Copying the block in order to format it before writing it in the buffer.
	FSBlock b = this->block;
	b.before_writting();
	BasicBuffer<sizeof(FSBlock)> buffer;
	buffer.add<FSBlock>(b);

	// Moving to the position of this object in the VFS and overwriting the block with the content of the buffer.
	std::ofstream str(this->refer_to.vfs_path(), std::ios::out | std::ios::in | std::ios::binary);
	if (str.is_open()){
		str.seekp(this->block_pos);
		buffer.write_to(str);
		str.close();
	}
	else{
		std::cerr << "The VFS could not be open in " << this->block.name << std::endl;
		return 1;
	}

	return 0;
}


/**
 * Refreshes FSObject::block (FSBlock) from the VFS.
 * The FSObject::block_pos is used to fetch only the required data.
 * Checks if this object was removed in the VFS.
 * 
 * | Returns 0 on success.
 * | Returns 1 if the object is marked as being removed in the VFS.
 * | Returns 2 if the VFS could not be opened.
 */
int FSObject::reload_from_vfs(){
	std::ifstream str(this->refer_to.vfs_path(), std::ios::binary | std::ios::in);
	if (str.is_open()){
		str.seekg(this->block_pos);
		str.read((char*)&this->block, sizeof(FSBlock));
		str.close();

		this->block.after_reading();

		if (removed_raised(this->block.flag)){
			std::cerr << "The object " << this->block.name << " was removed" << std::endl;
			return 1;
		}

		SystemName sn{this->block.id, this->block.extension};
		this->system_name = sn.c_str();
	}
	else{
		std::cerr << "Failed to open VFS" << std::endl;
		return 2;
	}

	return 0;
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


FSObject::FSObject(FileSystem& fs): refer_to(fs){}


FSObject::FSObject(FileSystem& fs, const FSBlock& bck, const FSPos& pos): block_pos(pos), block(bck), refer_to(fs){
	SystemName sn{this->block.id, this->block.extension};
	this->system_name = sn.c_str();
}
