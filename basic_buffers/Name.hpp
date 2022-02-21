#ifndef BASIC_NAME_HPP_INCLUDED
#define BASIC_NAME_HPP_INCLUDED

#include "StackSegment.hpp"
#include "LUT_chars.h"

#define SIZE_OF_NAME 32

class Name : public StackSegment<SIZE_OF_NAME>{

	void validate();

public:

	void override(const void* src, const size_t& s);

	Name();
	Name(const char* c);

};

inline void valid_name_char(uint8_t& c){
	c = LUT_Chars[c];
}

#endif // BASIC_NAME_HPP_INCLUDED