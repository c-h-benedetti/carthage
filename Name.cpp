#include "Name.hpp"
#include <cstring>

void Name::valid(char* buffer, bool dot) const{
	for (size_t i = 0 ; i < NAME_SIZE ; i++){
		char c = this->name[i];
		if ((c >= 65) && (c <= 90)){ // capital letter
			buffer[i] = c + 32;
		}
		else if((c >= 48) && (c <= 57)){ // digit
			buffer[i] = c;
		}
		else if((c >= 97) && (c <= 122)){
			buffer[i] = c;
		}
		else if(dot && (c == 46)){
			buffer[i] = c;
		}
		else if(c == 0){
			buffer[i] = 0;
		}
		else{
			buffer[i] = 95;
		}
	}
}

uint8_t Name::length() const{
	uint8_t i = 0;
	while ((i < NAME_SIZE) && (this->name[i])){
		i++;
	}
	return i;
}

void Name::get(char* buffer) const{
	for (size_t i = 0 ; i < NAME_SIZE ; i++){
		buffer[i] = this->name[i];
	}
}


Name Name::get_valid(bool dot) const{
	char buffer[NAME_SIZE];
	this->valid(buffer, dot);
	return Name{buffer};
}


Name::Name(){}


Name::Name(const char* c){
	memcpy(this->name, c, NAME_SIZE);
}


Name::Name(const Name& n){
	memcpy(this->name, n.name, NAME_SIZE);
}


Name& Name::operator=(const char* c){
	memcpy(this->name, c, NAME_SIZE);
	return *this;
}


Name& Name::operator=(const Name& c){
	memcpy(this->name, c.name, NAME_SIZE);
	return *this;
}
