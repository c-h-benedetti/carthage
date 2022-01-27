#include <iostream>
#include <fstream>
#include <chrono>
#include "MatchDirectories.hpp"

int main(int argc, char* argv[], char* env[]){

	for(uint i = 0 ; i < 200 ; i++){
		std::ifstream stream("long_blend.blend");
		MatchDirectories m{};

		auto start = std::chrono::high_resolution_clock::now();
		m.process(stream, 1);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		stream.close();
		std::cout << duration.count() << std::endl;
	}

	return 0;
}

