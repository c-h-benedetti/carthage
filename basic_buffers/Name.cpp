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
