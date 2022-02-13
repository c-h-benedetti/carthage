#include "FSObject.hpp"
#include "FileSystem.hpp"
#include <fstream>

fs::path FSObject::build_path() const{
	fs::path p = this->owner->base_path / CARTHAGE_DIR / DESCRIPTOR;
	std::ifstream stream(p.c_str(), std::ios::binary | std::ios::in);

	fs::path fp = "";
	FSBlock b = this->block;
	FPos current = this->fpos;
	char buffer[sizeof(Name) + 1] = {};

	while (current){
		b.name.valid(buffer);
		fp = buffer / fp;

		current = b.parent_address;
		stream.seekg(current);
		stream.read((char*)&b, sizeof(FSBlock));
	}

	fp = this->owner->data_root / fp;
	
	return fp;
}

void FSObject::load(){
	std::ifstream stream(this->owner->descriptor.c_str(), std::ios::binary | std::ios::in);

	if (stream.is_open()){

		stream.seekg(this->fpos);
		stream.read((char*)&this->block, sizeof(FSBlock));

		stream.close();
	}
}

FSObject::FSObject(){
	
}

FSObject::FSObject(FileSystem* o) : owner(o){

}

FSObject::FSObject(const FPos& pos, FileSystem* o) : fpos(pos), owner(o){
	
}

FSObject::FSObject(const FPos& pos, const FSBlock& b, FileSystem* o): fpos(pos), block(b), owner(o){

}