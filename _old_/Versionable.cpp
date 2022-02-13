#include "Versionable.hpp"

Versionable::Versionable(FileSystem* o): Folder(o){}

Versionable::Versionable(const FPos& pos, FileSystem* o): Folder(pos, o){}

Versionable::Versionable(const FPos& pos, const FSBlock& b, FileSystem* o): Folder(pos, b, o){}