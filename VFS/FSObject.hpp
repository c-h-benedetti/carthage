#ifndef FSOBJECT_HPP_INCLUDED
#define FSOBJECT_HPP_INCLUDED

#include "FSBlock.hpp"
#include "arg_blocks.hpp"

class FileSystem;
class Container;

class FSObject{

// IMPROVE: [FSObject] Check that the basic copy constructor doesn't produce messed up objects.
// IMPROVE: [FSObject] Make a method returning a copy with a different ID, empty content, ...

protected: 
	
	/// Reference to the FileSystem instance which generated this object.
	FileSystem& refer_to;
	/// Position of the data block in the VFS.
	FSPos block_pos = 0;
	/// Raw data block.
	FSBlock block;
	/// Pointer to the object considered as parent of this one.
	Container* previous = nullptr; 
	/// System path of this object.
	Path system_path;        

private:

	/// Tries to build a system path for this object.
	Path deduce_path() const;
	/// Describes the steps to load the element.
	virtual int load(){ return 0; }

public:

	/// Opens this element
	virtual int open();
	/// Fetch this block from the VFS and overrides FSObject::block.
	int reload_from_vfs();
	/// Write FSObject::block into the VFS at the position FSObject::block_pos.
	int override_vfs() const;

	bool is_current() const;
	bool is_valid() const;

	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	// ~    ACCESSORS                                                                ~
	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

	// ~ ~ > Getters

	inline const FileSystem& get_owner() const{ return this->refer_to; }
	inline const FSPos get_block_position() const{ return this->block_pos; }
	inline const FSBlock& get_block() const{ return this->block; }
	inline const Container* get_previous() const{ return this->previous; }
	inline const Path& get_system_path() const { return this->system_path; }	

	// ~ ~ > Setters

	inline void set_block_position(const FSPos& pos){ this->block_pos = pos; }
	inline FSBlock& editable_block(){ return this->block; }
	inline void set_previous(Container* c){ this->previous = c; }

	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	// ~    INSTANCIATION                                                            ~
	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

	FSObject() = delete;
	FSObject(FileSystem& fs, const FSPos& pos=0, Container* link=nullptr);
	FSObject(FileSystem& fs, const FSBlock& bck, const FSPos& pos=0, Container* link=nullptr);

	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	// ~    EXTRAS                                                                   ~
	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

	friend class FileSystem;
};

#endif // FSOBJECT_HPP_INCLUDED
