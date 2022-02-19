#include "Folder.hpp"
#include "FileSystem.hpp"
#include <fstream>

void Folder::open(){
	this->refer_to->change_directory(std::unique_ptr<Folder>(new Folder(*this)));
	this->refer_to->open();
}


FSObject* Folder::at(size_t i){
	if (i < folders.size()){
		return &(this->folders[i]);
	}
	i -= folders.size();
	if (i < files.size()){
		return &(this->files[i]);
	}
	i -= files.size();
	return &(this->versionables[i]);
}


size_t Folder::size() const{
	return this->folders.size() + this->files.size() + this->versionables.size();
}


void Folder::dispatch(const std::vector<FSObject>& segment){
	this->folders.clear();
	this->folders.reserve(this->block.nb_folders);

	this->files.clear();
	this->files.reserve(this->block.nb_files);

	this->versionables.clear();
	this->versionables.reserve(this->block.nb_versionables);


	for(size_t i = 0 ; i < segment.size() ; i++){
		switch (segment[i].data().flag){
			case(FSType::FILE):
				this->files.push_back(File(segment[i]));
				break;
			case(FSType::FOLDER):
				this->folders.push_back(Folder(segment[i]));
				break;
			case(FSType::VERSIONABLE):
				this->versionables.push_back(Versionable(segment[i]));
				break;
		}
	}
}


Folder::Folder(const Path& p, FileSystem* fs): Container(p, fs){}

Folder::Folder(const FSBlock& bck, const FSPos& pos, FileSystem* fs): Container(bck, pos, fs){}

Folder::Folder(const FSObject& obj): Container(obj){}

/*void Folder::add_file(const SystemName& name, FileSystem* fs){
	FSBlock b;
	b.id = ID(1); // Add a method to generate an ID without using the constructor
	b.name = name; // operator() = copy 
	b.flag = FSType::FILE;
	b.extension = Extension("test"); // operator() = copy 
	b.parent = this->block_pos;

	BasicBuffer<2*sizeof(FSPos)+sizeof(FSBlock)> buffer;
	buffer.add<FSize>(1);
	buffer.add<FSBlock>(b);
	buffer.add<FSPos>(0);

	std::ofstream o_str(fs->vfs_path, std::ios::out | std::ios::binary | std::ios::in);
	o_str.seekp(0, std::ios_base::end);
	FSPos insert = o_str.tellp();
	buffer.write_to(o_str);

	this->block.nb_files++;
	bool empty = false;

	if (!this->block.content){
		this->block.content = insert;
		empty = true;
	}

	o_str.seekp(this->block_pos);
	this->block.write(o_str);
	o_str.close();

	if(!empty){
		std::ifstream i_str(fs->vfs_path, std::ios::binary | std::ios::in);
		FSPos c_pos = this->block.content;
		FSize seg_size = 0;

		while(c_pos){
			i_str.seekg(c_pos);
			i_str.read((char*)&seg_size, sizeof(FSize));
			i_str.seekg(i_str.tellg() + seg_size * sizeof(FSBlock));
			i_str.read((char*)&c_pos, sizeof(FSPos));
		}

		FSPos last = i_str.tellg() - sizeof(FSPos);
		i_str.close();

		std::ofstream n_str(fs->vfs_path, std::ios::out | std::ios::binary | std::ios::in);
		n_str.seekp(last);
		n_str.write((char*)&insert, sizeof(FSPos));
		n_str.close();		
	}
}

void Folder::add_folder(const SystemName& name, FileSystem* fs){

}

void Folder::add_files(const std::vector<SystemName>& names, FileSystem* fs){

}

void Folder::add_folders(const std::vector<SystemName>& names, FileSystem* fs){

}*/