#include "ID.hpp"
#include "LUT_IDs.h"
#include <random>


ID::ID(int a){
	this->randomize();
}

void ID::randomize(){
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<uint8_t> dist(0, SIZE_IDS_ENTRY - 1);
	
	for (size_t i = 0 ; i < SIZE_OF_ID ; i++){
		this->buffer[i] = LUT_IDs[dist(mt)];
	}
}