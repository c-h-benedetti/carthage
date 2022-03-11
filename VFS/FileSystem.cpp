#include "FileSystem.hpp"
#include "File.hpp"
#include "Versionable.hpp"
#include <fstream>
#include <string>

// IMPROVE: [FileSystem] Update the content of this block along the implementation of new features

/**
 * Function returning the block every VFS starts with.
 */
static FSBlock file_system_default(const char* n){
	FSBlock b;

	b.content = 0;
	b.id.randomize();
	b.name = (n) ? (n) : ("Data");
	b.flag = pair_flags(FSType::ROOT, FSType::FOLDER);

	return b;
}

/**
 * Creates a new empty VFS from scratch.
 * Only the Carthage's hierarchy and a root are added (which is the minimal)
 */
int FileSystem::new_vfs(const char* n){

	// = = = 1. Creating the root of the VFS. The first block, in the first segment. = = =
	FSBlock block = file_system_default(n);

	// = = = 2. Creating the data's folder on the system, according to the newly created block. = = =
	SystemName name(block.id);
	Path data = this->io_manager.user_root / name.c_str();

	if (!fs::create_directory(data)){
		return 2;
	}

	block.time = fs::last_write_time(data);

	// = = = 3. Inserting a segment in the VFS file. The VFSWriter constructor will create it if it doesn't exist. = = =
	return this->io_manager.ask_writer(true).sequence(0).add_fsize(1).add_fsblock(block).add_fspos(0).status();
	// IMPROVE: [FileSystem] Error handling on output in new_vfs()
}



// DEBUG: [FSBlock] Check why calling constructor Name{0} or Extension{0} makes a segmentation fault.

/**
 * Method used to load the very first segment of a VFS.
 * The first segment is constant in term of size.
 * Its size is always of 1, and its next pointer is to 0.
 */
int FileSystem::open_root(){
	
	// = = = 1. Creating a new Folder object and filling it with the first block of the VFS. = = =
	this->stack.push_back(
		new Folder{
			*this,
			FSBlock(),
			FIRST_BLOCK_POSITION
		}
	);
	
	// Replacing the blank FSBlock with the one from the VFS
	if (this->current()->reload_from_vfs()){
		BasicBuffer<128> msg(0);
		msg.copy_from("Impossible to load the root from the VFS.");
		throw std::logic_error(msg.c_str());
	}
	
	// Checking that the flag is correct.
	if (removed_raised(this->current()->get_data().flag)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The root block of the VFS seems to be corrupted, its REMOVED flag is raised.");
		throw std::logic_error(msg.c_str());
	}
	
	// = = = 2. Opening the root as a regular Folder object. = = =
	return this->current()->load();
}



/**
 * Builds a FileSystem object.
 * p: Root path of Carthage's target. Should point to a directory.
 * reset: If a Carthage setup already exists, should it be overriden?
 * n: Name to give to the project. Can be nullptr.
 */
FileSystem::FileSystem(const Path& p, bool reset, const char* n): io_manager(p){
	
	if (reset || !fs::exists(this->io_manager.vfs_path)){
		// Creating a basic Carthage hierarchy and a minimal VFS file containing the root segment.
		if (this->new_vfs(n)){
			// IMPROVE: [FileSystem] Should throw a more suitable type of exception.
			throw std::invalid_argument("FileSystem::FileSystem: Failed to create a new VFS.");
		}
	}
	
	// Opening the first segment contained in the VFS file.
	if (this->open_root()){
		throw std::invalid_argument("FileSystem::FileSystem: Failed to open the VFS.");
	}
}

// IMPROVE: [FileSystem] Rebuild hierarchy functions

/*
static void recursive_hierarchy(size_t depth, const FSBlock& block, std::ifstream& vfs, std::ofstream& str){
	BasicBuffer<2048> buffer(0);

	// Opening the first tag containing the whole line.
	buffer.copy_from("<p class='line");

	if (root_raised(block.flag)){
		buffer.copy_from(" ROOT");
	}
	if (removed_raised(block.flag)){
		buffer.copy_from(" REMOVED");
	}
	if (file_raised(block.flag)){
		buffer.copy_from(" FILE");
	}
	if (folder_raised(block.flag)){
		buffer.copy_from(" FOLDER");
	}
	if (versionable_raised(block.flag)){
		buffer.copy_from(" VERSIONABLE");
	}
	if (current_raised(block.flag)){
		buffer.copy_from(" CURRENT");
	}
	if (version_raised(block.flag)){
		buffer.copy_from(" VERSION");
	}
	buffer.copy_from("'>");

	// Characters used to represent the hierarchy in theur own span
	buffer.copy_from("<span class='shift'>");
	if (depth >= 1){
		for (size_t i = 0 ; i < depth - 1 ; i++){
			buffer.copy_from("&#x2502;&nbsp;&nbsp;&nbsp;");
		}
	}
	if (depth > 0){
		buffer.copy_from("&#x251C;&#x2500;&#x2500; ");
	}
	buffer.copy_from("</span>");

	// Data representing the block
	buffer.copy_from("<span class='data'>");

	buffer.copy_from(block.name.c_str());

	if (file_raised(block.flag)){
		buffer.copy_from(" (").copy_from(block.extension.c_str()).copy_from(")");
	}
	if (folder_raised(block.flag) || versionable_raised(block.flag)){
		// Insert the formated number of elements.
	}
	buffer.copy_from("</span>");

	// Flag representing the type of the item.
	buffer.copy_from("<span class='hint'>");
	buffer.copy_from("  [");

	if (root_raised(block.flag)){
		buffer.copy_from("R");
	}
	else{
		buffer.copy_from(".");
	}
	if (removed_raised(block.flag)){
		buffer.copy_from("X");
	}
	else{
		buffer.copy_from(".");
	}
	if (file_raised(block.flag)){
		buffer.copy_from("f");
	}
	else{
		buffer.copy_from(".");
	}
	if (folder_raised(block.flag)){
		buffer.copy_from("F");
	}
	else{
		buffer.copy_from(".");
	}
	if (versionable_raised(block.flag)){
		buffer.copy_from("V");
	}
	else{
		buffer.copy_from(".");
	}
	if (current_raised(block.flag)){
		buffer.copy_from("C");
	}
	else{
		buffer.copy_from(".");
	}
	if (version_raised(block.flag)){
		buffer.copy_from("v");
	}
	else{
		buffer.copy_from(".");
	}

	buffer.copy_from("]");
	buffer.copy_from("</span>");

	buffer.copy_from("</span>");

	buffer.copy_from("</p>");

	buffer.copy_from("\n");

	str.write(buffer.c_str(), strlen(buffer.c_str()));

	if (block.content){
		std::vector<FSBlock> blocks;
		FSPos c_pos = block.content;
		FSize size = 0;

		while(c_pos){
			vfs.seekg(c_pos);
			vfs.read((char*)&size, sizeof(FSize));
			FSBlock read[size];
			blocks.reserve(blocks.size() + size);
			vfs.read((char*)read, sizeof(FSBlock)*size);

			for (size_t i = 0 ; i < size ; i++){
				read[i].after_reading();
				blocks.push_back(read[i]);
			}

			vfs.read((char*)&c_pos, sizeof(FSPos));
		}

		for(const FSBlock& blc : blocks){
			recursive_hierarchy(depth+1, blc, vfs, str);
		}
	}
}


void FileSystem::make_hierarchy(const Path& out) const{
	std::ofstream str(out, std::ios::out | std::ios::binary);
	std::ifstream vfs(this->vfs_file, std::ios::in | std::ios::binary);

	BasicBuffer<4096> buffer(0);
	BasicBuffer<4096> buffer2(0);

	buffer.copy_from("<!doctype HTML>\n");
	buffer.copy_from("<html>\n");
	buffer.copy_from("<head>\n");
	buffer.copy_from("    <link rel='stylesheet' type='text/css' href='vfs.css'>\n");
	buffer.copy_from("</head>\n");
	buffer.copy_from("<body>\n");
	buffer.write_to(str);

	vfs.seekg(sizeof(FSize));
	FSBlock block;
	vfs.read((char*)&block, sizeof(FSBlock));
	block.after_reading();

	recursive_hierarchy(0, block, vfs, str);

	buffer2.copy_from("</body>\n");
	buffer2.copy_from("</html>\n");
	buffer2.write_to(str);

	str.close();
	vfs.close();
}*/