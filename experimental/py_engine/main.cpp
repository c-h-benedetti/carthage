#include <iostream>
#include "PyEngine.hpp"

int main(int argc, char* argv[], char* env[]){

	PyEngine py_engine{"embedded_interpreter"};
	if (py_engine.run_string("print('Print from embedded Python')")){
		std::cerr << "Failed to run instruction" << std::endl;
	}

	PyInt i1{3};

	return 0;
}

// clear && g++ -Wall -std=c++20 $(python3.6-config --cflags --ldflags) -fPIC py_emb.cpp -lpython3.6 -o EMB && ./EMB

/*

[!] PyModule, PyUnicodeStr, PyFunc
	()

*/