#ifndef FSTYPES_DEFINITION_HEADER_INCLUDED
#define FSTYPES_DEFINITION_HEADER_INCLUDED

#include <cinttypes>

/// Type used in the enum of FSType
using FlagType = uint8_t;

/**
 * Representation of all possible base types for a block.
 * Also used to represent if a block was deleted.
 * A flag can be the union of multiple values.
 */
enum class FSType : FlagType{
    UNKNOWN = 0,
	REMOVED = 1 << 0,
	ROOT = 1 << 1,
	FILE = 1 << 2,
	FOLDER = 1 << 3,
	VERSIONABLE = 1 << 4,
	CURRENT = 1 << 5,
	VERSION = 1 << 6
};

inline bool is_undefined(const FSType& f){ return static_cast<int>(f) == 0; }
inline bool root_raised(const FSType& f){ return static_cast<FlagType>(FSType::ROOT) & static_cast<FlagType>(f); }
inline bool removed_raised(const FSType& f){ return static_cast<FlagType>(FSType::REMOVED) & static_cast<FlagType>(f); }
inline bool file_raised(const FSType& f){ return static_cast<FlagType>(FSType::FILE) & static_cast<FlagType>(f); }
inline bool folder_raised(const FSType& f){ return static_cast<FlagType>(FSType::FOLDER) & static_cast<FlagType>(f); }
inline bool versionable_raised(const FSType& f){ return static_cast<FlagType>(FSType::VERSIONABLE) & static_cast<FlagType>(f); }
inline bool current_raised(const FSType& f){ return static_cast<FlagType>(FSType::CURRENT) & static_cast<FlagType>(f); }
inline bool version_raised(const FSType& f){ return static_cast<FlagType>(FSType::VERSION) & static_cast<FlagType>(f); }

inline FSType pair_flags(const FSType& f1, const FSType& f2){ return static_cast<FSType>((FlagType)f1 | (FlagType)f2); }

inline bool is_valid(const FSType& t){
    return (!removed_raised(t)) && (file_raised(t) || folder_raised(t) || versionable_raised(t));
}


/**
 * 
 * If a block is deleted from the VFS, its block is simply edited to give it the value FSType::REMOVED. 
 * The length of the VFS file is not edited and the data of the block is still inside.
 * An element is really removed from the VFS:
 *	- Locally: If a defragmentation of the VFS is performed
 *	- Locally: A removed block might be deleted when merging from remote server if we are precisely fetching this block, or data related to this block.
 *	- Remotely: When receiving data from a user, removed elements are not transmitted.
 * 
 */

#endif // FSTYPES_DEFINITION_HEADER_INCLUDED