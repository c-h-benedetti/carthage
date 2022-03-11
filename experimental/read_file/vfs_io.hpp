#ifndef VFS_IO_OPERATIONS_HPP_INCLUDED
#define VFS_IO_OPERATIONS_HPP_INCLUDED

#include <iostream>
#include <fstream>
#include <memory>
#include "temp.h"

class VFSWriter{

	bool& release;
	bool corrupted = false;
	std::ofstream* stream = nullptr;

private:

	VFSWriter() = delete;
	VFSWriter(std::ofstream* str, bool& r): release(r), stream(str){}

	template <typename T>
	FSize write(const T& t);

public:

	VFSWriter& set_pos(const FSPos& pos); // perform seekp().

	VFSWriter& write_size(const FSize& s);
	VFSWriter& write_block(const FSBlock& block);
	VFSWriter& write_address(const FSPos& pos);

	inline ~VFSWriter(){
		this->stream->close();
		this->release = false;
	}

	VFSWriter(const VFSWriter& w) = delete;
	VFSWriter(VFSWriter&& w) = delete;
	VFSWriter& operator=(const VFSWriter& w) = delete;
	VFSWriter& operator=(VFSWriter&& w) = delete;


	friend class OVFS;
};




class OVFS{

	fs::path vfs_path;
	std::ofstream* stream = nullptr;
	bool busy = false; // Will need a condition variable or something

public:

	OVFS() = delete;
	OVFS(const fs::path& p);

	~OVFS(){
		if (this->stream){
			this->stream->close();
			this->stream = nullptr;
			this->busy = false;
		}
	}

	inline VFSWriter writer(){
		// A condition variable or a mutex must be used here.
		// If busy is true, must wait until it was false
		// If busy is false, do the following instructions:
		this->busy = true;
		this->stream->open(this->vfs_path, std::ios::in | std::ios::out | std::ios::binary);
		return VFSWriter(this->stream, this->busy);
	}

};


#include "vfs_io.tpp"

#endif // VFS_IO_OPERATIONS_HPP_INCLUDED

/*

The OVFS will be responsible for dispatching the stream through multi-threading. It must not give an instance to anybody asking it.

The FileSystem will hold a reference to a OVFS and to a IVFS, the owner will be the Project.
The FileSystem must ignore the existance of the Project class, but must know that xVFS classes exist.

Implement the mutex on the IO operations.

These classes will also be responsible for creating lock files OR lock blocks in the VFS to secure NAS installs.

*/