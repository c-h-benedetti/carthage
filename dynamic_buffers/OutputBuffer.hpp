#ifndef DYNAMIC_OUTPUT_BUFFER_HPP_INCLUDED
#define DYNAMIC_OUTPUT_BUFFER_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <functional>

class OutputBuffer {

	std::vector<char> data;

private:
	
	void init();

public:
	
	template <typename T>
	OutputBuffer& add(const T& t);

	OutputBuffer& copy_from(const void* c);
	OutputBuffer& copy_from(const void* c, const size_t& s);
	void write_to(std::ostream& os) const;
	inline void reset(){ this->data.clear();}

	OutputBuffer();
	OutputBuffer(const size_t& s);
};


template <typename T>
class BeforeWriting {

public:

	static std::function<void(T&)> behavior;
};

// Neutral function
template <typename T>
std::function<void(T&)> BeforeWriting<T>::behavior = [](T& i){};

#include "OutputBuffer.tpp"

#endif // DYNAMIC_OUTPUT_BUFFER_HPP_INCLUDED