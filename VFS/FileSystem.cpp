#include "FileSystem.hpp"
#include "Folder.hpp"
#include <fstream>
#include <string>


// IMPROVE: Update the content of this block along the implementation of new features
static FSBlock file_system_default(const char* n){
	FSBlock b;

	b.content = 0;
	b.id.randomize();

	if (n){
		b.name = n;
	}
	else{
		b.name = "Data";
	}

	b.flag = FSType::ROOT;
	b.icon = 1;

	return b;
}


void FileSystem::new_vfs(const char* n){

	// Creating the root of the VFS, the first block in the first segment.
	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> segment;
	FSBlock block = file_system_default(n);
	block.before_writting();
	segment.add<FSize>(1).add<FSBlock>(block).add<FSPos>(0);

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


void FileSystem::open(){
	// Reloading the block from the VFS to check for updates or deletions.
	this->current_obj->reload_from_vfs();
	// Reloading content of the container.
	this->current_obj->load();
	// Stacking for previous() and next() features.
	this->stack.push_back(FSObject(*(this->current_obj)));
}


FileSystem::FileSystem(const Path& p, bool reset, const char* n) : 
	current_path(p),
	user_root(p),
	vfs_file(p / CARTHAGE_DIR / VFS_DESCRIPTOR)
{

	if (!fs::exists(p)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The path \"").copy_from(p.c_str()).copy_from("\" doesn't exist.");
		throw std::invalid_argument(msg.c_str());
	}

	if (reset || !fs::exists(this->vfs_file)){
		this->new_vfs(n);
	}

	// Opening the first segment contained in the VFS file.
	this->open_root();
}


void FileSystem::open_root(){
	// The first segment is supposed to be constant in size.
	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> segment;

	// Reading the first segment of the VFS.
	std::ifstream str(this->vfs_file, std::ios::in | std::ios::binary);
	
	if (str.is_open()){
		segment.read_from(str);
		str.close();
	}
	else{
		BasicBuffer<128> msg(0);
		msg.copy_from("Failed to open the VFS to: ").copy_from(this->vfs_file.c_str());
		throw std::ios_base::failure(msg.c_str());
	}

	// Retreiving the prefix, the block and the suffix of the segment.
	FSize ssize = 0;
	FSPos next_b = 0;
	FSBlock block;

	BufferReader head(segment);
	head.get<FSize>(ssize).get<FSBlock>(block).get<FSPos>(next_b);

	block.after_reading();

	// The root is presumed to be unique so: size == 1
	// The root is the root (duh) so: next == 0
	// The root in constant and can't be unstacked and should always be present (so the removed flag not raised).
	if ((ssize != 1) || (next_b != 0) || (block.flag != FSType::ROOT)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The root's block of the VFS seems to be corrupted.");
		throw std::logic_error(msg.c_str());
	}

	this->current_obj = std::unique_ptr<Folder>(new Folder{
		block,
		sizeof(FSize), 
		this
	});

	this->current_path /= this->current_obj->path;
	this->current_obj->load();
}


void FileSystem::change_directory(std::unique_ptr<Container>&& c){
	this->current_obj = std::move(c);
}


void FileSystem::next(){

}


void FileSystem::previous(){

}

