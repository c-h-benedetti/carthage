#include "FSType.hpp"
#include "SystemName.hpp"
#include "FileSystem.hpp"
#include "File.hpp"
#include "Versionable.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

int FSObject::open(){
	if (this->reload_from_vfs()){
		return 6;
	}

	if (file_raised(this->block.flag)){
		File file(*this);

		if (file.open()){
			return 1;
		}
	}
	else if (folder_raised(this->block.flag)){
		this->refer_to.add(new Folder(*this));
		return this->refer_to.open();
	}
	else if (versionable_raised(this->block.flag)){
		this->refer_to.add(new Versionable(*this));
		return this->refer_to.open();
	}
	else{
		return 5;
	}

	return 0;
}


bool FSObject::is_current() const{ 
	return this->refer_to.current() == this; 
}


bool FSObject::is_valid() const{
	if (removed_raised(this->block.flag) || is_undefined(this->block.flag)){
		return false;
	}
	else if(!fs::exists(this->system_path) || !this->refer_to.vfs_io().is_from(this->system_path)){
		return false;
	}
	else if(this->block_pos == 0){
		return false;
	}
	else{
		return true;
	}
}


Path FSObject::deduce_path() const{
	Path p = "";

	if (this->previous){
		SystemName sn{this->block.id};
		p = this->previous->system_path / sn.c_str();
	}
	else{
		std::vector<FSBlock> blocks;
		
		if (!this->refer_to.vfs_io().ask_reader().backtrack(blocks, this->block_pos).status()){
			p = this->refer_to.vfs_io().user_root;
			size_t b_size = blocks.size();
			
			for (size_t i = 0 ; i < b_size ; i++){
				SystemName sn{blocks[b_size - i - 1].id};
				p /= sn.c_str();
			}
		}
	}
	
	return p;
}


/**
 * Refreshes FSObject::block (FSBlock) from the VFS.
 * The FSObject::block_pos is used to fetch only the required data.
 * Checks if this object was removed in the VFS.
 * 
 * | Returns 0 on success.
 * | Returns 1 if failed or the object is marked as being removed in the VFS.
 */
int FSObject::reload_from_vfs(){
	return (this->refer_to.vfs_io().ask_reader().probe_fsblock(this->block_pos, this->block).status() || removed_raised(this->block.flag));
}



/**
 * Overrides the block in the VFS file from this instance's FSObject::block (FSBlock).
 * Only this block is affected, it is not a general refreshing of the VFS.
 * The position where to write is known through FSObject::block_pos.
 * 
 * | Returns 0 on success.
 * | Returns 1 if the VFS could not be opened.
 * | Returns 2 if the block_pos == 0 (nullptr for the VFS).
 */
int FSObject::override_vfs() const{
	return this->refer_to.vfs_io().ask_writer().override<FSBlock>(this->block_pos, this->block).status();
}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


FSObject::FSObject(FileSystem& fs, const FSPos& pos, Container* link): refer_to(fs), block_pos(pos), previous(link){
	if (pos){
		this->reload_from_vfs();
	}
	this->system_path = this->deduce_path();
}

// IMPROVE: [FSObject] This last constructor should be removed, we should find a way to use only the previous one which much safer in terms of data validity.
FSObject::FSObject(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* link): refer_to(fs), block_pos(pos), block(bck), previous(link), system_path(this->deduce_path()){}
