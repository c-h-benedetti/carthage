#ifndef READ_FILE_WITH_CALLBACK_HPP_INCLUDED
#define READ_FILE_WITH_CALLBACK_HPP_INCLUDED

#include <fstream>
#include <functional>
#include <vector>

void read_callback(
	std::ifstream& stream, 
	std::function<void(const char*, const size_t&)> cb,
	const size_t& chunk,
	bool clear = true,
	bool close = true);

template <typename T>
void standard_reading(std::istream& s, std::vector<T>& v, bool clear = true);

#include "io_callback.tpp"

#endif // READ_FILE_WITH_CALLBACK_HPP_INCLUDED