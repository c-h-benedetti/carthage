#include "OutputBuffer.hpp"

void OutputBuffer::init(){

	BeforeWriting<int>::behavior = [](int& i){
		i *= 1;
	};

	BeforeWriting<char>::behavior = [](char& i){
		i *= 1;
	};

	BeforeWriting<float>::behavior = [](float& i){
		i *= 1;
	};

}

OutputBuffer::OutputBuffer(){
	this->init();
}

OutputBuffer::OutputBuffer(const size_t& s){
	this->init();
	this->data.clear();
	this->data.reserve(s);
}


OutputBuffer& OutputBuffer::copy_from(const void* c){
	const char* as_array = (const char*)c;
	const size_t s = strlen(as_array);

	for (size_t i = 0 ; i < s ; i++){
		this->data.push_back(as_array[i]);
	}

	return *this;
}


OutputBuffer& OutputBuffer::copy_from(const void* c, const size_t& s){
	const char* as_array = (const char*)c;

	for (size_t i = 0 ; i < s ; i++){
		this->data.push_back(as_array[i]);
	}

	return *this;
}


void OutputBuffer::write_to(std::ostream& os) const{
	os.write(this->data.data(), this->data.size());
}