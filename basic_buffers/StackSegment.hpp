#ifndef STACK_SEGMENT_HPP_INCLUDED
#define STACK_SEGMENT_HPP_INCLUDED

#include <iostream>
#include <cstring>
#include <cinttypes>

template <size_t N>
class StackSegment{

	static_assert ( N > 0, "The size of a stack segment cannot be 0");

protected:
	
	uint8_t buffer[N];

public:

	/// Default (empty) constructor. Doesn't do anything.
	StackSegment() = default;

	/// Builds the segment and initializes it with N times the c value. (Equivalent to memset)
	StackSegment(const uint8_t& c);

	/// Copy another segment into this segment. All bytes are copied, not only the used ones.
	StackSegment(const StackSegment<N>& b);

	/// Builds a segment from a  pointer of data and the number of bytes that we can read from it.
	StackSegment(const void* c, const size_t& s); 

	/// Overloading of copy constructor
	StackSegment<N>& operator=(const StackSegment<N>& b);

	/// Initializes the segment from a string.
	StackSegment<N>& operator=(const void* str);

	StackSegment(StackSegment<N>&& b) = delete;
	StackSegment<N>& operator=(StackSegment<N>&& b) = delete;

	/// The length of the buffer is the equivalent of strlen.
	inline size_t length() const{ 
		if (this->buffer[N-1]){
			return N;
		}
		else{
			return strlen((char*)this->buffer); 
		}
	}

	/// Argument passed as template, maximal size of the buffer.
	constexpr size_t size() const{ return N; }
	
	/// Sets all the bytes of the buffer to the null character.
	void nullify();

	/// Overrides the content of this buffer with some other data.
	void override(const void* src, const size_t& s);

	/// Checks if the content of two buffers of equal size is the same.
	bool equals(const StackSegment<N>& s2) const;

	/// Reads as many characters as possible from a stream.
	void read_from(std::istream& s);

	/// Writes as many characters as possible in a stream.
	void write_to(std::ostream& o) const;
	void copy(void* dest) const;

	void trim(const size_t& pos, const uint8_t& r=0);
	void untail();

	template <size_t K>
	friend std::ostream& operator<<(std::ostream& o, const StackSegment<K>& s);

	template <size_t K>
	friend std::istream& operator>>(std::istream& o, StackSegment<K>& s);

	inline uint8_t& operator[](const size_t& i){return this->buffer[i];}
	inline uint8_t operator[](const size_t& i) const{return this->buffer[i];}
	inline const char* c_str() const{ return (const char*)this->buffer; };
	inline const uint8_t* data() const{ return this->buffer; }
};

template <size_t N>
inline bool operator==(const StackSegment<N>& s1, const StackSegment<N>& s2){
	return s1.equals(s2);
}

template <size_t N>
inline bool operator!=(const StackSegment<N>& s1, const StackSegment<N>& s2){
	return !s1.equals(s2);
}

#include "StackSegment.tpp"

#endif // STACK_SEGMENT_HPP_INCLUDED

/*

[?] It could be great if we could copy a buffer to another, even if they are different sizes
[ ] The NULL-ID could be a constexpr declared in the header
[ ] We also need a function reading a file by chunks, it is the most repetitive & verbose code of Carthage's source code

*/

