#include "FileSystem.hpp"
#include <fstream>


const Path& Container::vfs_path() const{
	return this->refer_to.vfs_path();
}


/**
 * Function loading the content of a Container object.
 * Starts by collecting every single block present in the VFS.
 * Delegates the sorting of collected items to Container::dispatch().
 */
void Container::load(){
	std::vector<FSObject> collected;
	this->collect_content(collected);
	this->dispatch(collected);
}


/**
 * Tries to build a valid system name from a block.
 * Handles the presence of extension.
 * The FSBlock in argument is not constant because its ID can require to be randomized.
 */
Path Container::make_path(FSBlock& b) const{
	// Build the first try of a name. If extension is empty, no extension is added.
	SystemName sn(b.id, b.extension);
	Path abs_path = this->refer_to.current_syspath() / sn.c_str();

	// If the path already exists on the system, we try to re-randomize the ID.
	while (fs::exists(abs_path)){
		b.id.randomize();
		SystemName sn2(b.id, b.extension);
		abs_path = this->refer_to.current_syspath() / sn.c_str();
	}

	return abs_path;
}


/**
 * Links the last inserted block(s) to the data already present in the VFS.
 * Either by filling the FSBlock::content, or by browsing the VFS to update the value of the "next" pointer.
 * 
 * | Returns 0 on success.
 * | Returns 1 in case of failing to read from the VFS.
 * | Returns 2 in case of failing to write to the VFS.
 */
int Container::linking_segment(const FSPos& insert){
	
	if (this->block.content == 0){ // This object is an empty Container.
		this->block.content = insert;
	}
	else{
		FSPos c_pos = this->block.content; // Position of the content of this Container in the VFS file.
		FSPos last = this->block_pos; // Position of the NULL next pointer, the end of the last segment. FSBlock::content is the first element of the block, no shift required
		FSize seg_size = 0; // Size of the segment to be extracted at this iteration.

		std::ifstream i_str(this->refer_to.vfs_path(), std::ios::in | std::ios::binary);

		if (!i_str.is_open()){
			std::cerr << "The VFS could not be opened." << std::endl;
			return 1;
		}

		while(c_pos){
			// 1. Going to the position hinted by the pointer
			i_str.seekg(c_pos);
			// 2. Reading the size of this segment
			i_str.read((char*)&seg_size, sizeof(FSize));
			// 3. Position of the pointer "next" of this segment
			last = c_pos + sizeof(FSize) + sizeof(FSBlock) * seg_size;
			// 4. Skipping the content of the segment to reach the next pointer.
			i_str.seekg(last);
			// 5. Reading the position of the next segment.
			i_str.read((char*)&c_pos, sizeof(FSPos));
		}

		i_str.close();

		// Linking the new block to its parent
		std::ofstream l_str(this->refer_to.vfs_path(), std::ios::in | std::ios::out | std::ios::binary);

		if (!l_str.is_open()){
			std::cerr << "The VFS could not be opened." << std::endl;
			return 2;
		}

		// Moving to the NULL next pointer and overriding it with the position of the newly appended segment.
		l_str.seekp(last);
		l_str.write((char*)&insert, sizeof(FSPos));
		l_str.close();
	}

	return 0;
}


/**
 * Checks if a name already exist among the content of a Container.
 * Several objects can have the same name if they have different extensions.
 * 
 * | Returns true if the name already exists, false otherwise.
 */
bool name_exists(const Name& n, const Extension& ext){
	size_t i = 0;
	bool found = false;

	while ((i < this->size()) && !found){
		found = (this->at(i).data().name == n) && (this->at(i).data().extension == ext);
		i++;
	}

	return found;
}


// IMPROVE: [Container] The process to browse the VFS from an object could be refactored in one function. It could use lambda-expressions to do specific tasks.
/**
 * Function browsing the VFS to gather the content of a Container object.
 * Gets everything even removed blocks.
 */
int Container::collect_content(std::vector<FSObject>& segment){
	FSPos c_pos = this->block.content;
	FSize seg_size = 0;
	segment.clear();
	segment.reserve(this->block.nb_files + this->block.nb_folders + this->block.nb_versionables);

	std::ifstream str(this->refer_to.vfs_path(), std::ios::binary | std::ios::in);

	if (!str.is_open()){
		std::cerr << "Failed to open VFS" << std::endl;
		return 1;
	}

	// IMPROVE: [Container] We need to handle the endianness of the [segment's size] and the [next] pointer.
	while (c_pos){
		// 1. Reading the size of the segment
		str.seekg(c_pos);
		str.read((char*)&seg_size, sizeof(FSize));

		// 2. Extracting the blocks of this segment
		FSBlock blocks[seg_size];
		str.read((char*)blocks, seg_size * sizeof(FSBlock));

		// 3. Building FSObjects from the content.
		c_pos += sizeof(FSize); // Skip segment size to reach the position of the first block. 
		for (size_t i = 0 ; i < seg_size ; i++){
			blocks[i].after_reading();
			segment.push_back(FSObject(this->refer_to, blocks[i], c_pos));
			c_pos += sizeof(FSBlock);
		}

		// DEBUG: [Container] Add assert that checks if the reserved size matches the final size of the vector
		str.read((char*)&c_pos, sizeof(FSPos));
	}

	return 0;
}




// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Container::Container(FileSystem& fs): FSObject(fs){}

Container::Container(const FSObject& obj): FSObject(obj){}

Container::Container(FileSystem& fs, const FSBlock& bck, const FSPos& pos): FSObject(fs, bck, pos){}