#include "InputBuffer.hpp"

void InputBuffer::init(){
	// AfterReading<int>::behavior = [](int& i){
	// 	i *= 1;
	// };
}


InputBuffer::InputBuffer(){
	this->init();
}


void InputBuffer::read_from(std::istream& is, const size_t& sz){
	this->data.clear();
	this->head = 0;
	this->data.resize(sz);
	is.read(this->data.data(), sz);
}