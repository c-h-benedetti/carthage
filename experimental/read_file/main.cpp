#include <iostream>
#include "io_callback.hpp"

int main(int argc, char* argv[], char* env[]){

	std::ifstream s("/home/clement/Desktop/_carthage/read_file/io_callback.cpp");
	size_t chunk = 15;

	std::function<void(const char*, const size_t&)> callback = [&](const char* buffer, const size_t& size){
		std::cout << buffer;
		if (size != chunk){
			std::cout << std::endl << "=========" << std::endl << std::endl;
		}
	};

	read_callback(s, callback, chunk);

	return 0;
}