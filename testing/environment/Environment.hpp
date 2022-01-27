#ifndef GLOBAL_ENVIRONMENT_HPP_INCLUDED
#define GLOBAL_ENVIRONMENT_HPP_INCLUDED

#include <map>
#include <iostream>
#include "BasicCSV_IO.hpp"

enum class EnvType{
	INTEGER,
	CHARACTER,
	STRING,
	FLOATING,
	UNKNOWN
};

class Environment{

	void parse(BasicCSV_Reader& rd);

public:

	std::map<std::string, int> integers;
	std::map<std::string, std::string> strings;
	std::map<std::string, float> floats;
	std::map<std::string, char> chars;

public:

	Environment();
	Environment(const std::string& s);

	void tell_environment(std::ostream& o=std::cout) const;

	bool save();

};

#endif //GLOBAL_ENVIRONMENT_HPP_INCLUDED