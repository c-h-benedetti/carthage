#ifndef FSBLOCK_HPP_INCLUDED
#define FSBLOCK_HPP_INCLUDED

#include "utils.h"
#include "ID.hpp"
#include "Name.hpp"
#include "Extension.hpp"

// IMPROVE: [FSBlock] Implement the before_writting() and after_reading() methods.
// IMPROVE: [FSBlock] Correct the mispelled word: writting -> writing.
/**
 * Block representing an object in the VFS.
 * Methods after_reading() and before_writting() handle the system's endianness.
 * The content of this class, the order of the attributes and their size is made to avoid any padding on 32 and 64 bits systems.
 * An instance is always 152 bytes.
 * https://docs.google.com/spreadsheets/d/1qyhOJpsZgEd7bkQGaSMGHZQo6k2mXFJ7ZL9z3dDWHQg/edit?usp=sharing
 */
class FSBlock {

public:
	/// Position of the content of a Container in the VFS. Can be used with seekp() or seekg().
	FSPos content = 0;
	/// Unique ID of an object of the VFS: [a-z0-9]{SIZE_OF_ID}. Capital letters are not used in order to be able to use the id as the system's name on Windows as well.
	ID id;
	/// Displayed name of the object.
	Name name;
	/// If the object is a file, this contains its extension (txt, blend, ...)
	Extension extension;
	/// Number of folders in the container
	FSize nb_folders = 0;
	/// Number of files in the container
	FSize nb_files = 0;
	/// Number of versionable objects in the container
	FSize nb_versionables = 0;
	/// Icon of this object. 0 is reserved for processed icons (thumbnails)
	uint16_t icon = 1;
	/// Indicates the object's main category (file, folder, versionable). Also used to detect deletion.
	FSType flag = FSType::UNKNOWN;
	/// Permissions of group and other users on this object. The owner necessarily has all rights.
	uint8_t permissions = 0;
	/// When was this file created or edited?
	FSTime time;
	/// Position in the metadata file
	FSPos metadata = 0;
	/// Position of this object's parent in the VFS
	FSPos parent = 0;
	/// Who modified this object for the last time? 0 reserved for admin.
	UID editor = 0;
	/// Position in dependencies file.
	FSPos dependencies = 0;
	/// ID of the owner of this object. 0 reserved for admin
	UID owner = 0;
	/// ID of the group of this object. 0 reserved for no group.
	UID group = 0;


	FSBlock(){
		this->id.nullify();
		this->extension.nullify();
		this->name.nullify();
	}
};


static_assert(
	sizeof(FSPos) + 
	sizeof(ID) + 
	sizeof(Name) +
	sizeof(Extension) +
	3 * sizeof(FSize) +
	sizeof(uint16_t) +
	sizeof(FSType) +
	sizeof(uint8_t) +
	sizeof(FSTime) +
	sizeof(FSPos) +
	sizeof(FSPos) +
	sizeof(UID) +
	sizeof(FSPos) +
	2 * sizeof(UID) == sizeof(FSBlock),
	"Issue with padding of FSBlock."
);

static_assert(
	sizeof(FSBlock) == 152,
	"Size of FSBlock different from the expected one."
);


#endif // FSBLOCK_HPP_INCLUDED