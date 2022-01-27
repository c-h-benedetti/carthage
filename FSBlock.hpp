#ifndef FS_BLOCK_HPP_INCLUDED
#define FS_BLOCK_HPP_INCLUDED

#include "utils.h"

class FSBlock{

public:

	FPos content_address = 0;
	ID id;
	Name name;
	FSType type = FSType::UNKNOWN;
	FSize nb_folders = 0;
	FSize nb_files = 0;
	FSize nb_versionables = 0;
	uint32_t __blank__ = 0;
	FSTime last_edition_date;
	FPos metadata_address = 0;
	FPos parent_address = 0;
	UID last_editor = 0;
	Permissions permissions = 0;
	FileType ftype = 0;
	FPos dependencies_address = 0;
	UID owner = 0;
	UID group = 0;


	void after_reading_endianness();
	void before_writting_endianness();


	FSBlock();

	void tell() const;

};

#endif // FS_BLOCK_HPP_INCLUDED


/*

[!] At least for display purpose we will need a way to retreive a name from a UID, we also need to convert it from and to decimal form.

*/