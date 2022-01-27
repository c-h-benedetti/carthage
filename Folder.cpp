#include "Versionable.hpp"
#include <fstream>

Folder::Folder(FileSystem* o): FSObject(o){}

Folder::Folder(const FPos& pos, FileSystem* o): FSObject(pos, o){}

Folder::Folder(const FPos& pos, const FSBlock& b, FileSystem* o): FSObject(pos, b, o){}

Folder::~Folder(){
	delete[] this->files;
	delete[] this->folders;
	delete[] this->versionables;
}

void Folder::load(){
	this->FSObject::load();
	if (this->block.type != FSType::FOLDER){
		throw 20;
	}
	else{
		if (this->block.nb_folders){
			this->folders = new Folder[this->block.nb_folders];
		}
		if (this->block.nb_files){
			this->files = new File[this->block.nb_files];
		}
		if (this->block.nb_versionables){
			this->versionables = new Versionable[this->block.nb_versionables];
		}

		// [!] Add tests on fi, fo and v while reading to prevent overflow
		// [!] Read other necessary data segments
		// [!] Check if we could optimize reading
		// [!] Rebuild path of children here

		FSize fi = 0;
		FSize fo = 0;
		FSize v = 0;

		std::ifstream stream(this->owner.descriptor.c_str(), std::ios::binary | std::ios::in);
		FPos content = this->block.content_address;
		FSize segment_size = 0;

		while (content){
			stream.seekg(content);
			stream.read((char*)&segment_size, sizeof(FSize));
			FSBlock blocks[segment_size];
			stream.read((char*)blocks, segment_size * sizeof(FSBlock));

			for (FSize i = 0 ; i < segment_size ; i++){
				if ((blocks[i].type != FSType::REMOVED) && (blocks[i].type != FSType::UNKNOWN)){
					if ((blocks[i].type == FSType::FOLDER) || (blocks[i].type == FSType::ROOT)){
						this->folders[fo] = Folder(content + sizeof(FSize) + i * sizeof(FSBlock), blocks[i], this->owner);
						fo++;
					}
					else if(blocks[i].type == FSType::FILE){
						this->files[fi] = File(content + sizeof(FSize) + i * sizeof(FSBlock), blocks[i], this->owner);
						fi++;
					}
					else if(blocks[i].type == FSType::VERSIONABLE){
						this->versionables[v] = Versionable(content + sizeof(FSize) + i * sizeof(FSBlock), blocks[i], this->owner);
						v++;
					}
				}
			}

			stream.read((char*)&content, sizeof(FPos));
		}

		stream.close();
	}
}