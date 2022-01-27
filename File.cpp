#include "File.hpp"

File::File(): FSObject(){}

File::File(FileSystem* o): FSObject(o){}

File::File(const FPos& pos, FileSystem* o): FSObject(pos, o){}

File::File(const FPos& pos, const FSBlock& b, FileSystem* o): FSObject(pos, b, o){}