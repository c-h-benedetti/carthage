#ifndef FSOBJECT_HPP_INCLUDED
#define FSOBJECT_HPP_INCLUDED

#include "FSBlock.hpp"
#include "arg_blocks.hpp"

class FileSystem;
class Container;

class FSObject{

protected: 
	
	Path        system_name;        /// System name of this object (includes extension if it is a File).
	FSPos       block_pos = 0;      /// Position of the block in the VFS.
	FSBlock     block;              /// Raw data block.
	FileSystem& refer_to;           /// Reference of the FileSystem that generated this object.
	Container*  previous = nullptr; /// Object considered as parent of this one.

public:

	virtual int open(); /// Opens this element
	virtual int load(){ return 0; } /// Describes the steps to load the element.
	bool is_current() const;
	Path data_path(int* check=nullptr) const; /// Builds the data's path by following the @previous pointer
	Path deduce_path(int* check=nullptr) const;

	int reload_from_vfs(); /// Fetch this block from the VFS and overrides FSObject::block.
	int override_vfs() const; /// Write FSObject::block into the VFS at the position FSObject::block_pos.
	
	inline void chain(Container* prev){ /// Chains (== considers as parent) the #prev object.
		this->previous = prev;
	}

	inline void break_link(){ /// What to remove from global path when unstacking this object
		this->previous = nullptr;
	}

	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	// ~    ACCESSORS                                                                ~
	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

	// ~ ~ > Getters

	inline const FSBlock& get_data() const{ return this->block; }
	inline const Path& get_system_name() const { return this->system_name; }
	inline const Container* get_previous() const{ return this->previous; }

	// ~ ~ > Setters

	inline FSBlock& edit_data(){ return this->block; }
	inline void set_pos(const FSPos& pos){ this->block_pos = pos; }

	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	// ~    INSTANCIATION                                                            ~
	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

	FSObject() = delete;
	FSObject(FileSystem& fs);
	FSObject(FileSystem& fs, const FSBlock& bck, const FSPos& pos, Container* link=nullptr);

	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	// ~    EXTRAS                                                                   ~
	// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

	friend class FileSystem;
};

#endif // FSOBJECT_HPP_INCLUDED