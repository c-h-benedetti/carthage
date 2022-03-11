#include "vfs_io.hpp"

// This function will be responsible foor the endianness of what's read.
VFSWriter& VFSWriter::set_pos(const FSPos& pos){
	if (!this->corrupted){
		try{
			this->stream->seekp(pos);
		}
		catch(std::exception& e){
			this->corrupted = true;
		}
	}
	return *this;
}


// This function will be responsible foor the endianness of what's read.
VFSWriter& VFSWriter::write_size(const FSize& s){
	if (!this->corrupted){
		if (!this->write<FSize>(s)){
			std::cerr << "Fail to write" << std::endl;
		}
	}
	return *this;
}

// This function will be responsible for calling after_rading()
VFSWriter& VFSWriter::write_block(const FSBlock& block){
	if (!this->corrupted){
		if (!this->write<FSBlock>(block)){
			std::cerr << "Fail to write" << std::endl;
		}
	}
	return *this;
}


VFSWriter& VFSWriter::write_address(const FSPos& pos){
	if (!this->corrupted){
		if (!this->write<FSPos>(pos)){
			std::cerr << "Fail to write" << std::endl;
		}
	}
	return *this;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


OVFS::OVFS(const fs::path& p): vfs_path(p), stream(new std::ofstream()){

	if (!fs::exists(this->vfs_path)){
		std::ofstream i(this->vfs_path, std::ios::out | std::ios::trunc);

		if (!i.is_open()){
			throw "Failed to create an empty VFS";
		}
	}
}