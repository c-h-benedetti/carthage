#include "FileSystem.hpp"
#include "Folder.hpp"
#include <fstream>
#include <string>


// IMPROVE: Check if we can make this block more complete
static FSBlock file_system_default(){
	FSBlock b;

	b.content = 0;
	b.id.randomize();
	std::string name = "Data";
	b.name.override(name.c_str(), name.size());
	b.flag = FSType::ROOT;
	b.icon = 1;

	return b;
}


void FileSystem::new_vfs(){

	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> segment;
	FSBlock block = file_system_default();
	block.before_writing();
	segment.add<FSize>(1).add<FSBlock>(block).add<FSPos>(0);

	Path p = this->user_root / CARTHAGE_DIR;
	if (!fs::exists(p)){
		fs::create_directory(p);
	}

	Path data = this->user_root / (const char*)&SystemName(block.id, Extension{});
	if (!fs::exists(data)){
		fs::create_directory(data);
	}

	std::ofstream str(this->vfs_file, std::ios::out | std::ios::binary);
	
	if (str.is_open()){
		segment.write_to(str);
		str.close();
	}
	else{
		std::cerr << "Failed to create the new VFS" << std::endl;
	}
}


FileSystem::FileSystem(const Path& p, bool reset) : 
	current_path(p),
	user_root(p),
	vfs_file(p / CARTHAGE_DIR / VFS_DESCRIPTOR)
{

	if (reset || !fs::exists(this->vfs_file)){
		this->new_vfs();
	}
	
	this->open_root();
}


void FileSystem::open_root(){
	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> segment;

	std::ifstream str(this->vfs_file, std::ios::in | std::ios::binary);
	
	if (str.is_open()){
		segment.read_from(str);
		str.close();
	}
	else{
		std::cerr << "Failed to create the new VFS" << std::endl;
		return;
	}

	FSize size = 0;
	FSPos next = 0;
	FSBlock block;

	memcpy(&size, &segment, sizeof(FSize));
	memcpy(&block, &segment + sizeof(FSize), sizeof(FSBlock));
	memcpy(&next, &segment + sizeof(FSize) + sizeof(FSBlock), sizeof(FSPos));

	block.after_reading();

	if ((size != 1) || (next != 0) || (block.flag == FSType::REMOVED)){
		std::cerr << "The format of the root is corrupted" << std::endl;
		return;
	}

	this->current_obj = std::make_unique<Container>(new Folder{
		block,
		sizeof(FSize), 
		this
	});
	
	this->current_path /= this->current_obj->path;
	this->current_obj.load();
}


void FileSystem::change_directory(std::unique_ptr<Container>&& c){
	this->current = std::move(c);
}


void FileSystem::next(){

}


void FileSystem::previous(){

}


void FileSystem::dispatch(const std::vector<FSObject>& segment){
	this->folders.clear();
	this->folders.reserve(this->block.nb_folders);

	for(size_t i = 0 ; i < segment.size() ; i++){
		if (segment[i].data().flag == FSType::FOLDER){
			this->folders.push_back(Folder(segment[i]));
		}
	}
}
