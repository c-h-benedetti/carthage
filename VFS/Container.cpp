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
		found = (this->at(i).get_data().name == n) && (this->at(i).get_data().extension == ext);
		i++;
	}

	return found;
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
int Container::create_element(const FSize& count, FSBlock& blc, std::function<void(const Path&, const FSPos&)>& instanciating){
	// 1. Checks that the name doesn't already exist in this Container
	if (this->name_exists(blc.name, blc.extension)){
		return 1;
	}
	
	// 2. Reserving segment(s) by the end of the VFS that will contain the new elements.
	FSPos insert = 0;

	if (this->refer_to.vfs_io().ask_writer().blank_segments(count, insert) || !insert){
		return 2;
	}
	
	// 3. Building the path according to the data in the new block.
	int status;
	Path abs_path = this->data_path(&status);
	
	if (status){
		abs_path = this->deduce_path(&status);
	}
	
	if (status || !fs::exists(abs_path)){
		return 3;
	}
	
	abs_path = this->refer_to.vfs_io().make_path(abs_path, blc);
	
	// 4. Executing operations specific to a type of objects (like creating the block).
	instanciating(abs_path, insert);

	// 5. Writing blocks into the VFS

	// IMPROVE: [Container] Handle linking error codes.
	this->link_segment(insert);

	if (this->override_vfs()){
		return 4;
	}

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
			FSObject obj(this->refer_to, blocks[i], c_pos + i * sizeof(FSBlock));
			obj.chain(this);
			collected.push_back(obj);
		}
	});

	this->dispatch(collected);

	return 0;
}
// IMPROVE: [Container] Make the copy constructor override the chaining pointer of the contained FSObject.

/**
 * Links the last inserted block(s) to the data already present in the VFS.
 * Either by filling the FSBlock::content, or by browsing the VFS to update the value of the "next" pointer.
 * 
 * | Returns 0 on success.
 * | Returns 1 in case of failing to read from the VFS.
 * | Returns 2 in case of failing to write to the VFS.
 */
int Container::link_segment(const FSPos& insert){
	
	if (this->block.content == 0){ // This object is an empty Container.
		this->block.content = insert;
	}
	else{

		FSPos last = 0;

		this->refer_to.vfs_io().ask_reader().browse_block_content(this->block.content, [&](const FSize& size, const FSBlock* blocks, const FSPos& c_pos, const FSPos& next){
			last = c_pos + (size * sizeof(FSBlock));
		});

		this->refer_to.vfs_io().ask_writer().override<FSPos>(last, insert);
	}

	return 0;
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Container::Container(FileSystem& fs): FSObject(fs){}

Container::Container(const FSObject& obj): FSObject(obj){}

Container::Container(FileSystem& fs, const FSBlock& bck, const FSPos& pos): FSObject(fs, bck, pos){}