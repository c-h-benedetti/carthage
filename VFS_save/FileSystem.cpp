#include "FileSystem.hpp"
#include "File.hpp"
#include "Versionable.hpp"
#include <fstream>
#include <string>

// IMPROVE: Update the content of this block along the implementation of new features

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
void FileSystem::new_vfs(const char* n){

	// Creating the root of the VFS, the first block in the first segment.
	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> segment;
	FSBlock block = file_system_default(n);

	// Creating the Carthage's hidden directory.
	Path p = this->user_root / CARTHAGE_DIR;
	if (!fs::exists(p)){
		fs::create_directory(p);
	}

	// Creating the data's root according to the newly created block.
	SystemName name(block.id);
	Path data = this->user_root / name.c_str();
	if (!fs::exists(data)){
		fs::create_directory(data);
	}
	block.time = fs::last_write_time(data);
	block.before_writting();
	segment.add<FSize>(1).add<FSBlock>(block).add<FSPos>(0);

	// Writing the new block in the VFS.
	std::ofstream str(this->vfs_file, std::ios::out | std::ios::binary);

	if (str.is_open()){
		segment.write_to(str);
		str.close();
	}
	else{
		BasicBuffer<128> msg(0);
		msg.copy_from("Failed to create the VFS to: ").copy_from(this->vfs_file.c_str());
		throw std::ios_base::failure(msg.c_str());
	}
}


// IMPROVE: [FileSystem] When maps will be implemented, pass an ID rather than an object. An index could also be used.
void FileSystem::open(const FSObject& obj){

	// IMPROVE: [FileSystem] Is reloading useful? Check deletion. Handle errors.
	this->stack.back()->reload_from_vfs();
	FSType flag = obj.data().flag;

	if ((flag == FSType::UNKNOWN) || removed_raised(flag)){
		// IMPROVE: [FileSystem] Get sure that we leave the FS in a stable state. Maybe that we should rewind the stack until finding a valid item.
		return;
	}

	// IMPROVE: [FileSystem] Files opening handling is not very clean.

	if (file_raised(flag)){
		File file(obj);
		file.load();
	}
	else{

		Container* ct = nullptr;

		if(folder_raised(flag)){
			ct = new Folder(obj);
		}
		else if(versionable_raised(flag)){
			ct = new Versionable(obj);
		}

		if (ct){
			
			if (this->stack.size()){
				ct.chain(this->stack.back());
			}

			ct->reload_from_vfs();
			ct->load();
			this->stack.push_back(ct);
		}
	}
}

// IMPROVE: [FileSystem] Refactorize the code between open(), previous() & next().

void FileSystem::next(){
	if (this->head < this->stack.size() - 1){

		this->head++;
		const FSObject& obj = this->stack[this->head];
		FSType flag = obj.data().flag;

		if (folder_raised(flag)){
			this->current_obj->join(this->current_path);
			this->current_obj = std::unique_ptr<Container>(new Folder(obj));
		}
		else if (versionable_raised(flag)){
			this->current_obj->join(this->current_path);
			this->current_obj = std::unique_ptr<Container>(new Versionable(obj));
		}
		else{
			// IMPROVE: [FileSystem] Improve error handling if the data type is unknown. Find a way to cancel operation to leave in stable state.
			return;
		}

		this->current_obj->reload_from_vfs();
		this->current_obj->load();
	}
}

// DEBUG: [General] Make a verbose mode with preprocessor blocks.

void FileSystem::previous(){
	if (this->head > 0){

		this->head--;
		const FSObject& obj = this->stack[this->head];
		FSType flag = obj.data().flag;

		if (folder_raised(flag)){
			this->current_obj->unjoin(this->current_path);
			this->current_obj = std::unique_ptr<Container>(new Folder(obj));
		}
		else if (versionable_raised(flag)){
			this->current_obj->unjoin(this->current_path);
			this->current_obj = std::unique_ptr<Container>(new Versionable(obj));
		}
		else{
			// IMPROVE: [FileSystem] Improve error handling if the data type is unknown. Find a way to cancel operation to leave in stable state.
			return;
		}

		this->current_obj->reload_from_vfs();
		this->current_obj->load();
	}
}


// DEBUG: [FSBlock] Check why calling constructor Name{0} or Extension{0} makes a segmentation fault.

/**
 * Method used to load the very first segment of a VFS.
 * The first segment is constant in term of size.
 * Its size is always of 1, and its next pointer is to 0.
 */
void FileSystem::open_root(){
	// IMPROVE: [FileSystem] Remplacer "sizeof(FSize)" par une variable de préprocesseur au cas où l'architecture change.
	this->current_obj = std::unique_ptr<Folder>(new Folder{
		*this,
		FSBlock(),
		sizeof(FSize)
	});

	this->current_obj->reload_from_vfs();

	if (removed_raised(this->current_obj->data().flag)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The root block of the VFS seems to be corrupted.");
		throw std::logic_error(msg.c_str());
	}

	this->stack.push_back(FSObject(*this->current_obj));
	this->current_obj->join(this->current_path);
	this->current_obj->load();
}


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
}

/**
 * Builds a FileSystem object.
 * p: Root path of Carthage's target. Should point to a directory.
 * reset: If a Carthage setup already exists, should it be overriden?
 * n: Name to give to the project. Can be nullptr.
 */
FileSystem::FileSystem(const Path& p, bool reset, const char* n): user_root(p),	vfs_path(p / CARTHAGE_DIR / VFS_DESCRIPTOR){

	// If the user-provided path doesn't exist or isn't a directory.
	if (!fs::exists(p) || !fs::is_directory(p)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The path \"").copy_from(p.c_str()).copy_from("\" doesn't exist, or is not a directory.");
		throw std::invalid_argument(msg.c_str());
	}

	if (reset || !fs::exists(this->vfs_file)){
		// Creating a basic Carthage hierarchy and a minimal VFS file containing the root segment.
		if (this->new_vfs(n)){
			// IMPROVE: [FileSystem] Should throw a more suitable type of exception.
			throw std::invalid_argument("Failed to create a new VFS.");
		}
	}

	// Opening the first segment contained in the VFS file.
	if (this->open_root()){
		throw std::invalid_argument("Failed to open the VFS.");
	}
}