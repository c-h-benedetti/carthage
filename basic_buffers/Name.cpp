#include "Name.hpp"

Name::Name(){
	this->nullify();
}

Name::Name(const char* c){
	this->override(c, strlen(c));
	this->validate();
}

void Name::validate(){
	for(size_t i = 0 ; i < SIZE_OF_NAME ; i++){
		valid_name_char(this->buffer[i]);
	}
}

bool Name::is_valid() const{
	return is_valid_string((const char*)this->buffer);
}

void Name::override(const void* src, const size_t& s){
	this->StackSegment::override(src, s);
	this->validate();
	this->buffer[SIZE_OF_NAME - 1] = 0;
}


bool is_valid_string(const char* c){
	size_t i = 0;
	while ((i < SIZE_OF_NAME) && (c[i] != 0)){
		if (!is_valid_char(c[i])){
			return false;
		}
		i++;
	}

	return true;
}