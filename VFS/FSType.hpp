#ifndef FSTYPES_DEFINITION_HEADER_INCLUDED
#define FSTYPES_DEFINITION_HEADER_INCLUDED

#include <cinttypes>

// IMPROVE: Add methods to check which flags are raised
enum class FSType : uint8_t{
	UNKNOWN,
	REMOVED,
	ROOT,
	FILE,
	FOLDER,
	VERSIONABLE
};


#endif // FSTYPES_DEFINITION_HEADER_INCLUDED