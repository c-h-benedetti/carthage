#include "Extension.hpp"

Extension::Extension(){
	this->nullify();
}

Extension::Extension(const char* c){
	this->override(c, strlen(c));
	for(size_t i = 0 ; i < SIZE_OF_EXTENSION ; i++){
		valid_ext_char(this->buffer[i]);
	}
}

void Extension::override(const void* src, const size_t& s){
	this->StackSegment::override(src, s);
	this->buffer[SIZE_OF_EXTENSION - 1] = 0;
}