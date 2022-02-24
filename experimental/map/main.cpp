#include <iostream>
#include "Map.hpp"
#include <string>
#include <cstring>

int main(int argc, char* argv[], char* env[]){

	std::function<bool(const std::string&, const std::string&)> f = [&](const std::string& s1, const std::string& s2){
		return strcmp(s1.c_str(), s2.c_str()) < 0;
	};

	std::vector<std::string> keys = {"e", "g", "d", "c", "a", "f", "b"};
	std::vector<int> values = {0, 1, 2, 3, 4, 5, 6};

	Map<std::string, int> map(keys, values, f);

	std::function<void(const size_t&, const std::string&, const int&)> disp = [&](const size_t& i, const std::string& s, const int& v){
		std::cout << s << ": " << v << std::endl;
	};

	map.const_for_each(disp);

	return 0;
}