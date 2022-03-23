#include "FSType.hpp"
#include "FileSystem.hpp"

/**
 * Checks if a name already exist among the content of a Container.
 * Several objects can have the same name if they have different extensions.
 * Lazy evaluation and earliest possible break are used to make this function optimized.
 * -> However, this function will be faster when the new type of map will be introduced.
 * 
 * | Returns true if the name already exists, 
 * | false otherwise.
 */
bool Container::name_exists(const Name& n, const Extension& ext) const{
	size_t i = 0;
	bool found = false;

	while ((i < this->size()) && !found){
		const FSBlock& block = this->read(i).get_block();
		found = (block.name == n) && (block.extension == ext);
		i++;
	}

	return found;
}


bool Container::accepts(const FSType& flag) const{
	if(file_raised(flag)){
		return this->accepts_files();
	}
	else if(folder_raised(flag)){
		return this->accepts_folders();
	}
	else if(versionable_raised(flag)){
		return this->accepts_versionables();
	}
	else{
		return false;
	}
}


/**
 * Skeleton of a function creating a new element in the VFS.
 * Operations common to all objects are factorized here.
 * The lambda-expression passed as argument must just create the blocks, and add the object to the collection.
 * | Returns 0 on success.
 * | Returns 1 if an object with the same name already exists.
 * | Returns 2 if it is impossible to add blank segments in the VFS.
 * | Returns 3 if all attemps to create a valid path failed.
 * | Returns 4 or 8 if the new blocks could not be written in the VFS.
 */
int Container::create_element(const FSize& count, FSBlock& blc, std::function<void(const Path&, const FSPos&, FSBlock& modif)>& instanciating){
	// 1. If the name already exists, we instantly abort.
	if (this->name_exists(blc.name, blc.extension)){
		return 1;
	}
	
	if (this->reload_from_vfs() || !this->is_valid()){
		return 3;
	}
	// IMPROVE: [Container] Should we block the access to the VFS once this method started? What happens if this block is modified during this function?
	// IMPROVE: [Container] The update of counters should be done during linking, we should not override blindly the VFS in case another thread added content at the same time. (a write-locker must be acquired)
	// IMPROVE: [Container] When we link, the writing lock must be acquired before the linking starts.
	
	// 2. Reserving blank segment(s) at the end of the VFS that will contain the new elements.
	FSPos insert = 0;
	if (this->refer_to.vfs_io().ask_writer().blank_segments(count, insert).status() || !insert){
		return 2;
	}
	
	// 3. Building the path according to the data in the new block.
	Path abs_path = this->get_system_path();
	// IMPROVE: [VFS_IO] The make_path() function should check some safety conditions.
	abs_path = this->refer_to.vfs_io().make_path(abs_path, blc);
	
	// 4. Executing operations specific to a type of objects (like creating the block).
	FSBlock edition;
	instanciating(abs_path, insert, edition);

	// 5. Writing blocks into the VFS

	// IMPROVE: [Container] Handle linking error codes.
	this->refer_to.vfs_io().ask_writer().link_segment(this->block_pos, this->block, insert, edition);

	return 0;
}



/**
 * Function loading the content of a Container object.
 * Starts by collecting every single block present in the VFS.
 * Delegates the sorting of collected items to Container::dispatch().
 */
int Container::load(){
	std::vector<FSObject> collected;

	this->refer_to.vfs_io().ask_reader().browse_block_content(this->block.content, [&](const FSize& size, const FSBlock* blocks, const FSPos& c_pos, const FSPos& next){
		for (size_t i = 0 ; i < size ; i++){
			collected.push_back(FSObject(this->refer_to, blocks[i], c_pos + i * sizeof(FSBlock), this));
		}
	});

	this->dispatch(collected);

	return 0;
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Container::Container(const FSObject& obj): FSObject(obj){}

Container::Container(FileSystem& fs, const FSPos& pos, Container* previous): FSObject(fs, pos, previous){}

Container::Container(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* previous): FSObject(fs, bck, pos, previous){}