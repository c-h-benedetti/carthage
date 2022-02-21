#include <iostream>
#include <cmath>
#include <unistd.h>
#include "Log.hpp"

int main(int argc, char* argv[], char* env[]){

	Log& l = Log::log();
	l.time_stamp = true;

	Log::add_stream("test.log");

	for (size_t i = 0 ; i < 50 ; i++){
		l.add("Un string pas formatté\n");
		l.add("Je sais pas quelle est la limite de taille\n");
		l.add(snprintf(l.buffer(), l.buffer_size(), "Premier: %d %d %d; \n", (int)i, (int)i, (int)i)).write();
		l.add(snprintf(l.buffer(), l.buffer_size(), "Autre: %d \n", 42)).write();
		l.add(snprintf(l.buffer(), l.buffer_size(), "3\n")).write();
		l.add(snprintf(l.buffer(), l.buffer_size(), "4\n")).write();
	}
	
	Log::terminate();

	return 0;
}