#ifndef BASIC_BUFFER_HPP_INCLUDED
#define BASIC_BUFFER_HPP_INCLUDED

#include "StackSegment.hpp"

template <size_t K>
class BasicBuffer : public StackSegment<K> {

	size_t pos = 0;

public:

	inline size_t position() const { return this->pos; }
	void clear();
	inline bool is_full() const { return this->pos >= K; }

	template <typename T>
	BasicBuffer<K>& add(const T& t);

	BasicBuffer<K>& add(const void* src);
	BasicBuffer<K>& add(const void* src, const size_t& sz);
};

#include "Buffer.tpp"

#endif // BASIC_BUFFER_HPP_INCLUDED