#ifndef DYNAMIC_INPUT_BUFFER_HPP_INCLUDED
#define DYNAMIC_INPUT_BUFFER_HPP_INCLUDED

#include <iostream>
#include <functional>
#include <vector>

class InputBuffer {

	std::vector<char> data;
	size_t head = 0;

private:

	void init();

public:

	inline bool is_empty() const{ return this->data.size() >= this->head; }
	inline void reset(){ this->data.clear(); this->head = 0;}
	void read_from(std::istream& is, const size_t& sz);

	template <typename T>
	InputBuffer& get(T& t);

	InputBuffer();
};


// This template class will be cloned for each type declared at compile-time.
template <typename T>
class AfterReading {

public:

	static std::function<void(T&)> behavior;
};


// Neutral function
template <typename T>
std::function<void(T&)> AfterReading<T>::behavior = [](T& i){};

#include "InputBuffer.tpp"

#endif // DYNAMIC_INPUT_BUFFER_HPP_INCLUDED