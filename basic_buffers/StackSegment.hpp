#ifndef STACK_SEGMENT_HPP_INCLUDED
#define STACK_SEGMENT_HPP_INCLUDED

#include <iostream>
#include <cstring>
#include <cinttypes>

template <size_t N>
class StackSegment{

	static_assert ( N > 0, "The size of the segment can't be 0");

protected:
	
	uint8_t buffer[N];

public:

	StackSegment() = default;
	StackSegment(const uint8_t& c);
	StackSegment(const StackSegment<N>& b);
	StackSegment(const void* c, const size_t& s); 
	StackSegment<N>& operator=(const StackSegment<N>& b);
	StackSegment<N>& operator=(const char* str);

	StackSegment(StackSegment<N>&& b) = delete;
	StackSegment<N>& operator=(StackSegment<N>&& b) = delete;

	inline size_t length() const{ 
		if (this->buffer[N-1]){
			return N;
		}
		else{
			return strlen((char*)this->buffer); 
		}
	}

	inline size_t size() const{ return N; }
	
	void nullify();
	void override(const void* src, const size_t& s);

	void read_from(std::istream& s);
	void write_to(std::ostream& o) const;
	void copy(void* dest) const;

	template <size_t K>
	friend std::ostream& operator<<(std::ostream& o, const StackSegment<K>& s);

	template <size_t K>
	friend std::istream& operator>>(std::istream& o, StackSegment<K>& s);

	inline uint8_t& operator[](const size_t& i){return this->buffer[i];}
	inline uint8_t operator[](const size_t& i) const{return this->buffer[i];}
	inline const char* c_str() const{ return (const char*)this->buffer; };
	inline const uint8_t* data() const{ return this->buffer; }
};


#include "StackSegment.tpp"

#endif // STACK_SEGMENT_HPP_INCLUDED

/*

[?] It could be great if we could copy a buffer to another, even if they are different sizes
[ ] The NULL-ID could be a constexpr declared in the header
[ ] We also need a function reading a file by chunks, it is the most repetitive & verbose code of Carthage's source code

*/

