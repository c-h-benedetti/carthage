#include "io_callback.hpp"
#include <cstring>

void read_callback(
	std::ifstream& stream, 
	std::function<void(const char*, const size_t&)> cb,
	const size_t& chunk,
	bool clear,
	bool close)
{
	char buffer[chunk] = {};
	bool go = true;

	while (go){
		if (clear){
			memset(buffer, 0, chunk);
		}
		go = !stream.read(buffer, chunk).eof();
		cb(buffer, stream.gcount());
	}

	if (close){
		stream.close();
	}
}