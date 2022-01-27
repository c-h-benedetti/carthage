#ifndef BASIC_NAME_HPP_INCLUDED
#define BASIC_NAME_HPP_INCLUDED

#include <cstdint>

#define NAME_SIZE 31

class Name{

	char name[NAME_SIZE] = {};

public:

	Name get_valid(bool dot=false) const;
	void valid(char* c, bool dot=false) const;
	void get(char* buffer) const;
	uint8_t length() const;

	Name();
	Name(const char* c);
	Name(const Name& n);

	Name& operator=(const char* c);
	Name& operator=(const Name& c);

};

#endif // BASIC_NAME_HPP_INCLUDED