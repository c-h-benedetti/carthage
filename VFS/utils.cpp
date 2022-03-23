#include <fstream>
#include "utils.h"
#include <cstring>

int touch(const Path& path){
	
	std::ofstream stream(path, std::ios::binary | std::ios::out);

	if (stream.is_open()){
		stream.close();
		return 0;
	}
	else{
		return 1;
	}
}

bool a_starts_with_b(const char* a, const char* b){
	const size_t sz_b = strlen(b);

	if (sz_b > strlen(a)){
		return false;
	}
	else{
		size_t i = 0;
		while ((i < sz_b) && (a[i] == b[i])){
			i++;
		}
		return (i == sz_b);
	}
}