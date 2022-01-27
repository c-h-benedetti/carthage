#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "ID.hpp"

void build_name(char* buffer){
	for (size_t i = 0 ; i < SIZE_ID ; i++){
		switch (rand() % 3){
			case (0):
				buffer[i] = static_cast<char>(rand() % (ascii_9 + 1 - ascii_0)  + ascii_0);
			break;
			case (1):
				buffer[i] = static_cast<char>(rand() % (ascii_Z + 1 - ascii_A)  + ascii_A);
			break;
			case (2):
				buffer[i] = static_cast<char>(rand() % (ascii_z + 1 - ascii_a) + ascii_a);
			break;
		};
	}
}

ID::ID(){
	build_name(this->id);
}

ID::ID(const char* c){
	memcpy(this->id, c, SIZE_ID);
}

ID::ID(const ID& i): ID(i.id){}

ID& ID::operator=(const ID& i){
	memcpy(this->id, i.id, SIZE_ID);
	return *this;
}

ID& ID::operator=(const char* i){
	memcpy(this->id, i, SIZE_ID);
	return *this;
}

void ID::tell() const{
	char bfr[SIZE_ID+1] = {};
	memcpy(bfr, this->id, SIZE_ID);
	printf("%s\n", bfr);
}

void ID::get(char* c) const{
	memcpy(c, this->id, SIZE_ID);
}

ID::ID(int i){}

ID ID::null_id(){
	return ID(0);
}

int idcmp(const ID& id1, const ID& id2){
	uint16_t i = 0;
	char c;
	const char* c1 = id1.id;
	const char* c2 = id2.id;

	while ((i < SIZE_ID) && ((c = c1[i] - c2[i]) == 0)){
		i++;
	}
	return c;
}

bool operator<(const ID& id1, const ID& id2){
	return idcmp(id1, id2) < 0;
}

bool operator>(const ID& id1, const ID& id2){
	return idcmp(id1, id2) > 0;
}

bool operator==(const ID& id1, const ID& id2){
	return idcmp(id1, id2) == 0;
}

bool operator<=(const ID& id1, const ID& id2){
	return idcmp(id1, id2) <= 0;
}

bool operator>=(const ID& id1, const ID& id2){
	return idcmp(id1, id2) >= 0;
}