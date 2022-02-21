#ifndef PY_ENGINE_HEADER_HPP_INCLUDED
#define PY_ENGINE_HEADER_HPP_INCLUDED

#include "PyInt.hpp"
#include <cstring>
#include <string>

class PyEngine {

	std::unique_ptr<wchar_t, std::function<void(wchar_t*)>> program;

public:

	int run_string(const std::string& s);

	PyEngine(const char* c = NULL);
	~PyEngine();
};

#endif // PY_ENGINE_HEADER_HPP_INCLUDED