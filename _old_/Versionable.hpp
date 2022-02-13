#ifndef VERSIONABLE_BASE_HPP_INCLUDED
#define VERSIONABLE_BASE_HPP_INCLUDED

#include "Folder.hpp"

class Versionable : public Folder{

public:

	Versionable(FileSystem* o);
	Versionable(const FPos& pos, FileSystem* o);
	Versionable(const FPos& pos, const FSBlock& b, FileSystem* o);

};

#endif // VERSIONABLE_BASE_HPP_INCLUDED