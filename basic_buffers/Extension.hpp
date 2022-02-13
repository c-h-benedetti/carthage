#ifndef EXTENSION_HPP_INCLUDED
#define EXTENSION_HPP_INCLUDED

#include "StackSegment.hpp"
#define SIZE_OF_EXTENSION 8

class Extension : public StackSegment<SIZE_OF_EXTENSION>{

	friend class SystemName;

public:

	Extension();
	Extension(const char* c);

};

inline void valid_ext_char(uint8_t& c){
	if ((c >= 65) && (c <= 90)){
		c += 32;
	}
}

#endif // EXTENSION_HPP_INCLUDED