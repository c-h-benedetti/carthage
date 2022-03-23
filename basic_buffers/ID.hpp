#ifndef BASIC_ID_HPP_INCLUDED
#define BASIC_ID_HPP_INCLUDED

#include <random>
#include <functional>
#include "StackSegment.hpp"

#define SIZE_OF_ID 32

class ID : public StackSegment<SIZE_OF_ID>{
	
	friend class SystemName;

	static std::random_device rd;
	static std::mt19937 mt;
	static std::uniform_int_distribution<uint8_t> dist;
	static std::function<uint8_t()> generator;
	
public:

	ID() = default;
	ID(int a);
	void randomize();

	void override(const void* src, const size_t& s) = delete;
};

inline bool operator<(const ID& id1, const ID& id2){
	return strcmp(id1.c_str(), id2.c_str()) < 0;
}

inline bool operator>(const ID& id1, const ID& id2){
	return strcmp(id1.c_str(), id2.c_str()) > 0;
}

#endif // BASIC_ID_HPP_INCLUDED