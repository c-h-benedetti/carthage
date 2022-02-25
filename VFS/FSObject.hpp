#ifndef FSOBJECT_HPP_INCLUDED
#define FSOBJECT_HPP_INCLUDED

#include "FSBlock.hpp"
#include "arg_blocks.hpp"

class FileSystem;

class FSObject{

protected: 
	
	Path        system_name;   /// System name of this object (includes extension if it is a File).
	FSPos       block_pos = 0; /// Position of the block in the VFS.
	FSBlock     block;         /// Raw data block.
	FileSystem& refer_to;      /// Reference of the FileSystem that generated this object.

public:

	virtual void load(){}          /// Describes the steps to load the element.
	virtual void join(Path& p){}   /// What to add to the global path to reach this element on the system.
	virtual void unjoin(Path& p){} /// What to remove from global path when unstacking this object

	int reload_from_vfs();    /// Fetch this block from the VFS and overrides FSObject::block.
	int override_vfs() const; /// Write FSObject::block into the VFS at the position FSObject::block_pos.

	inline const FSBlock& data() const{ return this->block; }

	FSObject() = delete;
	FSObject(FileSystem& fs);
	FSObject(FileSystem& fs, const FSBlock& bck, const FSPos& pos);

	friend class FileSystem;

};

#endif // FSOBJECT_HPP_INCLUDED