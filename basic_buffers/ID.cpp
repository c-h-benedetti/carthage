#include "ID.hpp"
#include "LUT_IDs.h"

std::random_device ID::rd;
std::mt19937 ID::mt = std::mt19937(rd());
std::uniform_int_distribution<uint8_t> ID::dist = std::uniform_int_distribution<uint8_t>(0, SIZE_IDS_ENTRY - 1);
std::function<uint8_t()> ID::generator = std::bind(ID::dist, std::ref(ID::mt));


ID::ID(int a){
	this->randomize();
}


void ID::randomize(){
	for (size_t i = 0 ; i < SIZE_OF_ID ; i++){
		this->buffer[i] = LUT_IDs[ID::generator()];
	}
}