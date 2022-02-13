#ifndef BASIC_ID_HPP_INCLUDED
#define BASIC_ID_HPP_INCLUDED

#include "StackSegment.hpp"

#define SIZE_OF_ID 32

class ID : public StackSegment<SIZE_OF_ID>{
	
	friend class SystemName;
	
public:

	ID() = default;
	ID(int a);

};

#endif // BASIC_ID_HPP_INCLUDED