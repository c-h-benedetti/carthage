#ifndef FSBLOCK_HPP_INCLUDED
#define FSBLOCK_HPP_INCLUDED

#include "utils.h"

class FSBlock {
	FSPos content = 0;
	ID id;
	Name name;
	FSType flag;
	Extension extension;
	FSize nb_folders = 0;
	FSize nb_files = 0;
	FSize nb_versionables = 0;
	uint16_t icon = 0;
	uint16_t permissions = 0;
	FSTime time;
	FSPos metadata = 0;
	FSPos parent = 0;
	UID editor = 0;
	FSPos dependencies = 0;
	UID owner = 0;
	UID group = 0;
};

static_assert(
	sizeof(FSPos) + 
	sizeof(ID) + 
	sizeof(Name) +
	sizeof(FSType) +
	sizeof(Extension) +
	3 * sizeof(FSize) +
	2 * sizeof(uint16_t) +
	sizeof(FSTime) +
	sizeof(FSPos) +
	sizeof(FSPos) +
	sizeof(UID) +
	sizeof(FSPos) +
	2 * sizeof(UID) == sizeof(FSBlock),
	"Issue with padding of FSBlock"
);

#endif // FSBLOCK_HPP_INCLUDED