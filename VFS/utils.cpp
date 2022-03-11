#include <fstream>
#include "utils.h"


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