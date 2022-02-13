#include "Project.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <fstream>
#include <cstring>

int main(int argc, char* argv[], char* env[]){

	srand(time(NULL));

	Projects projects{};
	
	projects.get_project("J1PvvB1NI2VUmxCH8ZtGj252kjqP483m");

	/*

	// * * * * * * * * * * * * * * *

	if (projects.new_project("My Awesome Project", "sessions")){
		std::cerr << "Failed to create new project" << std::endl;
	}

	if (projects.new_project("My Other Project", "sessions")){
		std::cerr << "Failed to create new project" << std::endl;
	}*/


	return 0;
}