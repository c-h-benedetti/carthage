#include "Name.hpp"

Name::Name(){
	this->nullify();
}

Name::Name(const char* c){
	this->override(c, strlen(c));
	this->validate();
	this->buffer[SIZE_OF_NAME - 1] = 0;
}

void Name::validate(){
	for(size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		valid_name_char(this->buffer[i]);
	}
}

void Name::override(const void* src, const size_t& s){
	this->StackSegment::override(src, s);
	this->buffer[SIZE_OF_NAME - 1] = 0;
}
