#include "FileSystem.hpp"
#include "File.hpp"
#include "Versionable.hpp"
#include <fstream>
#include <string>
#include <utility>

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
	return this->io_manager.ask_writer(true).sequence(0).add<FSize>(1).add<FSBlock>(block).add<FSPos>(0).status();
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


static Path join(const std::vector<Path>& paths){
	Path aggregate = "";
	for (const Path& item : paths){
		aggregate /= item;
	}

	return aggregate;
}


void FileSystem::create(const FSObject& src, const Path& src_path, const Path& dst_path){

	if (file_raised(src.get_data().flag)){
		fs::copy_file(src_path, dst_path);
	}
	else if(folder_raised(src.get_data().flag)){
		fs::create_directory(dst_path);
	}
	else if(versionable_raised(src.get_data().flag)){
		fs::create_directory(dst_path);
	}
}

/**
 * This function copies all blocks in #source to #dest.
 * A test is performed to verify that #dest accepts each block.
 * The recursive copy is launched after the segment was created.
 */
void FileSystem::copy_to(std::vector<FSObject>& sources, Container* dest){
	
	if (!dest){
		return;
	}

	std::vector<std::pair<FSObject, FSObject>> orig_and_copies;

	// Filtering objects that the destination could refuse.
	for (size_t i = 0 ; i < sources.size() ; i++){
		
		FSObject& orig = sources[i];
		orig.reload_from_vfs();

		if (is_valid(orig.get_data().flag)){
			FSBlock copy_data = orig.get_data();
			
			copy_data.id.randomize();
			copy_data.parent = dest->block_pos;
			copy_data.content = 0;

			FSObject copy(*this, copy_data, 0);
			copy.chain(dest);

			orig_and_copies.push_back({orig, copy});
			this->create(orig, orig.data_path(), copy.data_path());
		}
	}

	FSPos head = 0;
	this->io_manager.ask_writer().blank_segment(orig_and_copies.size(), head);
	OutputBuffer buffer;

	// Creating the new segment, and writing it to the VFS.
	buffer.add<FSize>(orig_and_copies.size());
	for (size_t i = 0 ; i < orig_and_copies.size() ; i++){
		std::pair<FSObject, FSObject>& obj = orig_and_copies[i];
		obj.second.set_pos(head + sizeof(FSize) + i * sizeof(FSBlock));
		buffer.add<FSBlock>(obj.second.get_data());
	}
	buffer.add<FSPos>(0);
	
	this->io_manager.ask_writer().sequence(head).write_to_vfs(buffer);
	dest->link_segment(head);

	for (size_t i = 0 ; i < orig_and_copies.size() ; i++){

		std::pair<FSObject, FSObject>& objs = orig_and_copies[i];

		if (folder_raised(objs.second.get_data().flag)){
			dest->edit_data().nb_folders++;
		}
		else if (versionable_raised(objs.second.get_data().flag)){
			dest->edit_data().nb_versionables++;
		}
		else{
			dest->edit_data().nb_files++;
		}

		this->replicate_to(objs.first, objs.second);
	}

	dest->override_vfs();
}


// IMPROVE: [FileSystem] We must be able to provide a writer and a reader at the same time. We will certainly have to make a system of partial locking for writing, with a range of bytes. In this case, we can issue several writers if they are on different ranges. Of the same way, readers can be issued if writers exist in the case where the range is not the same. A mutex must be locked the time that the VFS_IO takes its decision.
void FileSystem::replicate_to(const FSObject& source, FSObject& dst){

	// = = Check que la destination accepte le type de la source = =

	std::vector<std::pair<FSObject, FSObject>> stack;
	FSPos insert = 0;

	stack.push_back({
		source,
		dst
	});

	Path path_src = source.data_path();
	path_src = path_src.parent_path();

	Path path_dst = dst.data_path();

	while (stack.size() > 0){
		FSObject current = stack.back().first;
		FSObject current_dup = stack.back().second;
		stack.pop_back();

		if (is_undefined(current.get_data().flag)){
			path_src = path_src.parent_path();
			path_dst = path_dst.parent_path();
		}
		else{

			SystemName sn_src{current.get_data().id};
			SystemName sn_dst{current_dup.get_data().id};

			path_src /= sn_src.c_str();
			path_dst /= sn_dst.c_str();

			this->create(current, path_src, path_dst);

			// = = = = = = = = = = =

			stack.push_back({ // Délimiteur
				FSObject(*this, FSBlock(), 0),
				FSObject(*this, FSBlock(), 0)
			});

			// If the element doesn't contain anything (file or empty folder) we don't need to probe anything.
			if (current.get_data().content){
				bool at_least_once = false;
				std::vector<FSObject> collected;

				this->io_manager.ask_reader().browse_block_content(current.get_data().content, [&](const FSize& seg_size, const FSBlock* blocks, const FSPos& c_pos, const FSPos& next){
					for (size_t i = 0 ; i < seg_size ; i++){
						if (!removed_raised(blocks[i].flag)){
							at_least_once = true;
							collected.push_back(
								FSObject(*this, blocks[i], c_pos + i * sizeof(FSBlock), (Container*)&current)
							);
						}
					}
				});

				FSPos head = 0;
				this->io_manager.ask_writer().blank_segment(collected.size(), head);
				
				if (!insert){
					insert = head;
				}

				if (at_least_once){
					current_dup.edit_data().content = head;
				}

				OutputBuffer buffer;
				buffer.add<FSize>(collected.size());

				for (size_t i = 0 ; i < collected.size() ; i++){
					FSBlock new_data = collected[i].get_data();
					new_data.parent = current_dup.block_pos;
					new_data.id.randomize();
					new_data.content = 0;

					FSObject o(*this, new_data, head + sizeof(FSize) + i * sizeof(FSBlock), (Container*)&current_dup);
					
					buffer.add<FSBlock>(o.get_data());

					stack.push_back({
						collected[i],
						o
					});
				}

				buffer.add<FSPos>(0);

				this->io_manager.ask_writer().sequence(head).write_to_vfs(buffer);
				current_dup.override_vfs();
			}
		}
	}

	if (insert){
		Container* c = (Container*) &dst;
		c->link_segment(insert);
	}
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

	BasicBuffer<4096> buffer(0);
	BasicBuffer<4096> buffer2(0);

	buffer.copy_from("<!doctype HTML>\n");
	buffer.copy_from("<html>\n");
	buffer.copy_from("<head>\n");
	buffer.copy_from("    <link rel='stylesheet' type='text/css' href='vfs.css'>\n");
	buffer.copy_from("</head>\n");
	buffer.copy_from("<body>\n");
	buffer.write_to(str);

	FSBlock block;
	this->io_manager.ask_reader().sequence(FIRST_BLOCK_POSITION).read<FSBlock>(block);

	recursive_hierarchy(0, block, vfs, str);

	buffer2.copy_from("</body>\n");
	buffer2.copy_from("</html>\n");
	buffer2.write_to(str);

	str.close();
}
*/