#include "SystemName.hpp"
#include "FileSystem.hpp"
#include "File.hpp"
#include "Versionable.hpp"
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


/**
 * Builds the path of this block by following the @previous pointer until finding a nullptr.
 * The returned path can be invalid if the chain is broken.
 * Even on success, the returned path doesn't necessarily exist.
 * If #check points to an integer, it will contain the execution's status code.
 *  | 0: Success
 *  | 1: Error, the root was not found along the path.
 * 
 * Return value:
 *  | If the operation is a success (status code 0) the path is returned.
 *  | If the root is missing (status code 1), an empty path is returned.
 */
Path FSObject::data_path(int* check) const{
	// 1. Initialization by fetching the list of objects along the path.
	Path path = "";
	int status = 0;

	std::vector<const FSObject*> list;
	list.push_back(this);
	FSObject* prev = this->previous;

	// DEBUG: [FSObject] Check que root n'est rencontré qu'une fois, que le previous de la root est nullptr.
	while (prev){
		list.push_back(prev);
		prev = prev->previous;
	}

	// 2. If the last recorded object is not the root, the path won't be valid, we can abort.
	if (!root_raised(list.back()->get_data().flag)){
		status = 1;
	}
	else{
		// 3. Assembling the path.
		path = this->refer_to.vfs_io().user_root;
		// IMPROVE: [FSObject] Check if a way exists to reserve some place for the path.
		size_t l_size = list.size();
		for (size_t i = 0 ; i < l_size ; i++){
			path /= list[l_size - i - 1]->get_system_name();
		}
	}

	if (check){ *check = status; }

	return path;
}


Path FSObject::deduce_path(int* check) const{
	std::vector<FSBlock> blocks;
	int status = 0;
	Path p = "";

	if (this->refer_to.vfs_io().ask_reader().backtrack(blocks, this->block_pos)){
		status = 1;
	}
	else{
		p = this->refer_to.vfs_io().user_root;
		size_t b_size = blocks.size();
		for (size_t i = 0 ; i < b_size ; i++){
			SystemName sn{blocks[b_size - i - 1].id, blocks[b_size - i - 1].extension};
			p /= sn.c_str();
		}
	}

	if (check){ *check = status; }

	return p;
}



/**
 * Refreshes FSObject::block (FSBlock) from the VFS.
 * The FSObject::block_pos is used to fetch only the required data.
 * Checks if this object was removed in the VFS.
 * 
 * | Returns 0 on success.
 * | Returns 1 if the object is marked as being removed in the VFS.
 * | Returns 2 if the VFS could not be opened.
 * | Returns 3 if the block_pos == 0 (nullptr for the VFS).
 */
int FSObject::reload_from_vfs(){
	int status = this->refer_to.vfs_io().ask_reader().probe_fsblock(this->block_pos, this->block);

	if (status){
		return status;
	}
	else{
		// IMPROVE: [FSObject] If the object was deleted, the FileSystem must react and find a stable state.
		if (removed_raised(this->block.flag)){
			return 20;
		}
		else{
			SystemName sn{this->block.id, this->block.extension};
			this->system_name = sn.c_str();
			return 0;
		}
	}
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
	return this->refer_to.vfs_io().ask_writer().override<FSBlock>(this->block_pos, this->block);
}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


FSObject::FSObject(FileSystem& fs): refer_to(fs), previous(nullptr){}


FSObject::FSObject(FileSystem& fs, const FSBlock& bck, const FSPos& pos): block_pos(pos), block(bck), refer_to(fs), previous(nullptr){
	SystemName sn{this->block.id, this->block.extension};
	this->system_name = sn.c_str();
}
