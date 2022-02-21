#include "PyEngine.hpp"
#include <functional>

PyEngine::PyEngine(const char* c){
	if (c){
		this->program = std::unique_ptr<wchar_t, std::function<void(wchar_t*)>>(
			Py_DecodeLocale(c, NULL),
			[](wchar_t* pgm){
				PyMem_RawFree(pgm);
			}
		);

		if (!this->program || !this->program.get()) {
	        throw "Failed to convert program's name";
	    }

	    Py_SetProgramName(this->program.get());  // optional but recommended
	}

	Py_Initialize();
}


PyEngine::~PyEngine(){
	if (Py_FinalizeEx()){
		std::cerr << "Failed to terminate Python execution" << std::endl;
	}
	std::cout << "Embedded Python interpreter terminated" << std::endl;
}


int PyEngine::run_string(const std::string& s){
	return PyRun_SimpleString(s.c_str());
}