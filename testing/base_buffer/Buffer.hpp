#ifndef BASIC_BUFFER_HPP_INCLUDED
#define BASIC_BUFFER_HPP_INCLUDED


#include <iostream>
#include <cstring>


template <size_t N>
class Buffer{

	static_assert ( N > 0, "The size of the buffer can't be 0");

	char buffer[N];
	size_t current_size;

public:

	Buffer();
	Buffer(const Buffer<N>& b);
	Buffer(const void* c, const size_t& s); 
	Buffer<N>& operator=(const Buffer<N>& b);

	Buffer(Buffer<N>&& b) = delete;
	Buffer& operator=(Buffer<N>&& b) = delete;


	inline size_t size() const{ return this->current_size; }
	inline size_t max() const{ return N; }
	inline bool is_full() const{ return this->current_size == N; }
	inline bool is_empty() const{ return this->current_size == 0; }
	size_t add(const void* c, const size_t& s);
	void nullify();

	template <typename T>
	size_t add(const T& data);

	void read_from(std::istream& s);
	void get_all(void* dest, const size_t& s) const;
	void get(void* dest, const size_t& s) const;
	void write_all_to(std::ostream& o) const;
	void write_to(std::ostream& o) const;

};


constexpr bool power_of_two(size_t n) {
	uint size = sizeof(size_t);
	unsigned char slices[size] = {};
	*slices = n;
	uint count = 0;

	for (uint i = 0 ; i < size ; i++){
		switch (slices[i]){
			case(0):
			break;
			case (1 << 0):
				count++;
			break;
			case (1 << 1):
				count++;
			break;
			case (1 << 2):
				count++;
			break;
			case (1 << 3):
				count++;
			break;
			case (1 << 4):
				count++;
			break;
			case (1 << 5):
				count++;
			break;
			case (1 << 6):
				count++;
			break;
			case (1 << 7):
				count++;
			break;
			default:
				return false;
		}
		if (count > 1){
			return false;
		}
	}

	return true;
}


#include "Buffer.tpp"

#endif // BASIC_BUFFER_HPP_INCLUDED

/*

[?] It could be great if we could copy a buffer to another, even if they are different sizes
[ ] The NULL-ID could be a constexpr declared in the header
[ ] We also need a function reading a file by chunks, it is the most repetitive & verbose code of Carthage's source code

*/

