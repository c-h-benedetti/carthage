#include <iostream>
#include "io_callback.hpp"
#include "vfs_io.hpp"

void t1(){
	std::ifstream s("/home/clement/Desktop/carthage/experimental/read_file/io_callback.cpp");
	size_t chunk = 15;

	std::function<void(const char*, const size_t&)> callback = [&](const char* buffer, const size_t& size){
		std::cout << buffer;
		if (size != chunk){
			std::cout << std::endl << "=========" << std::endl << std::endl;
		}
	};

	read_callback(s, callback, chunk);
}

void t2(){

	OVFS vfs("/home/clement/Desktop/carthage/experimental/read_file/somename.txt");
	vfs.writer().write_block('a').write_block('b').write_block('c').write_block('d');

}

int main(int argc, char* argv[], char* env[]){

	t2();

	return 0;
}