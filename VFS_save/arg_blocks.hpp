#ifndef VFS_ARGUMENTS_BLOCKS_HPP_INCLUDED
#define VFS_ARGUMENTS_BLOCKS_HPP_INCLUDED

#include "arguments.hpp"

/**
 * Arguments required for the creation of a new file.
 */
class ArgsNewFile{

public:

	ArgFileName name;
	ArgFileExtension extension;
	ArgIcon icon;

};


/**
 * Arguments required for the creation of a new folder.
 */
class ArgsNewFolder{

public:

	ArgFolderName name;
	ArgIcon icon;
};


/**
 * Arguments required for the creation of a new versionable.
 */
class ArgsNewVersionable : public ArgsNewFolder{

};

#endif // VFS_ARGUMENTS_BLOCKS_HPP_INCLUDED