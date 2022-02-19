#include "FileSystem.hpp"
#include <fstream>
#include <string>

void FileSystem::open(){
	this->current->load_from_vfs();
	this->current->load();
}

size_t FileSystem::size() const{
	return this->folders.size();
}

FSObject* FileSystem::at(size_t i){
	return &(this->folders[i]);
}

void FileSystem::new_vfs(){

	fs::path p = this->path / CARTHAGE_DIR;
	if (!fs::exists(p)){
		fs::create_directory(p);
	}

	BasicBuffer<sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos)> segment;
	segment.add<FSize>(1).add<FSBlock>(this->block).add<FSPos>(0);

	std::ofstream str(this->vfs_path, std::ios::out | std::ios::binary);
	
	if (str.is_open()){
		segment.write_to(str);
		str.close();
	}
	else{
		std::cerr << "Failed to create the new VFS" << std::endl;
	}
}

// IMPROVE: Check if we can make this block more complete
static FSBlock file_system_default(){
	FSBlock b;
	b.content = 0;
	b.id.randomize();
	std::string name = "Root";
	b.name.override(name.c_str(), name.size());
	b.flag = FSType::ROOT;
	b.icon = 1;
	return b;
}

FileSystem::FileSystem(const Path& p, bool reset): 
	Container(p / DATA, file_system_default(), sizeof(FSize), this), 
	full_path(this->path),
	root(p),
	vfs_path(p / CARTHAGE_DIR / VFS_DESCRIPTOR)
{

	if (reset || !fs::exists(this->vfs_path)){
		this->new_vfs();
	}
	
	this->load_from_vfs(false);
	this->load();
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
