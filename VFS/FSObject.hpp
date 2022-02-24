#ifndef FSOBJECT_HPP_INCLUDED
#define FSOBJECT_HPP_INCLUDED

#include "FSBlock.hpp"
#include "arg_blocks.hpp"

class FileSystem;

class FSObject{

	friend class FileSystem;

protected: 
	/// System name of this object (includes extension if it is a File).
	Path path;
	/// Represents selection for batch operations
	bool selected = false;
	/// Position of the block in the VFS.
	FSPos block_pos = 0;
	/// Raw data block.
	FSBlock block;
	// IMPROVE: [FSObject] Make the FileSystem pointer a reference instead, since we can initialize it from the very begining.
	FileSystem* refer_to = nullptr;

protected:

	// IMPROVE: [FSObject] The load() method should be purely virtual, it doesn't have implementation.
	virtual void load(){}

public:

	inline void toggle_select(){ this->selected = !this->selected; }
	inline void set_selection(const bool& b){ this->selected = b; }
	inline const FSBlock& data() const{ return this->block; }

	// IMPROVE: [FSObject] The open() method should be purely virtual, it doesn't have implementation.
	virtual void open(){}
	void reload_from_vfs();
	void refresh_vfs() const;

	FSObject() = delete;
	FSObject(FileSystem* fs);
	FSObject(const FSBlock& bck, const FSPos& pos, FileSystem* fs);

};

#endif // FSOBJECT_HPP_INCLUDED