#ifndef SYSTEM_NAME_HPP_INCLUDED
#define SYSTEM_NAME_HPP_INCLUDED

#include "Extension.hpp"
#include "ID.hpp"

#define SIZE_OF_SYSNAME (SIZE_OF_ID + SIZE_OF_EXTENSION + 2)

class SystemName : public StackSegment<SIZE_OF_SYSNAME>{

public:
	
	SystemName(const ID& id, const Extension& ext);

};

#endif // SYSTEM_NAME_HPP_INCLUDED