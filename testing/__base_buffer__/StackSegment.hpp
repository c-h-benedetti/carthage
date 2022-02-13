#ifndef BASIC_BUFFER_HPP_INCLUDED
#define BASIC_BUFFER_HPP_INCLUDED


#include <iostream>
#include <cstring>


template <size_t N>
class StackSegment{

	static_assert ( N > 0, "The size of the segment can't be 0");

	uint8_t buffer[N];
	size_t current_size;

public:

	StackSegment();
	StackSegment(const StackSegment<N>& b);
	StackSegment(const void* c, const size_t& s); 
	StackSegment<N>& operator=(const StackSegment<N>& b);

	StackSegment(StackSegment<N>&& b) = delete;
	StackSegment& operator=(StackSegment<N>&& b) = delete;

	inline size_t size() const{ return this->current_size; }
	inline size_t max() const{ return N; }
	inline bool is_full() const{ return this->current_size == N; }
	inline bool is_empty() const{ return this->current_size == 0; }
	
	void nullify();
	
	template <typename T> 
	StackSegment<N>& add(const T& data);
	
	StackSegment<N>& add(const void* c, const size_t& s);
	void override(void* src, const size_t& s) const;

	void read_from(std::istream& s);
	void write_all_to(std::ostream& o) const;
	void write_to(std::ostream& o) const;
	void copy(void* dest) const;
	void copy_all(void* dest) const;

};


#include "StackSegment.tpp"

#endif // BASIC_BUFFER_HPP_INCLUDED

/*

[?] It could be great if we could copy a buffer to another, even if they are different sizes
[ ] The NULL-ID could be a constexpr declared in the header
[ ] We also need a function reading a file by chunks, it is the most repetitive & verbose code of Carthage's source code

*/

