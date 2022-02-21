#ifndef BASIC_BUFFER_HPP_INCLUDED
#define BASIC_BUFFER_HPP_INCLUDED

#include "StackSegment.hpp"

template <size_t K>
class BasicBuffer : public StackSegment<K> {

	size_t pos = 0;

public:

	BasicBuffer() = default;
	BasicBuffer(const uint8_t& c): StackSegment<K>(c){}

	inline size_t position() const { return this->pos; }
	void clear();
	inline bool is_full() const { return this->pos >= K; }

	template <typename T>
	BasicBuffer<K>& add(const T& t);

	BasicBuffer<K>& copy_from(const void* src);
	BasicBuffer<K>& copy_from(const void* src, const size_t& sz);
};


template <size_t K>
class BufferReader{

	size_t pos = 0;
	const BasicBuffer<K>* buffer;

public:

	BufferReader(const BasicBuffer<K>& buffer);

	template <typename T>
	BufferReader<K>& get(T& t);
};

#include "Buffer.tpp"

#endif // BASIC_BUFFER_HPP_INCLUDED