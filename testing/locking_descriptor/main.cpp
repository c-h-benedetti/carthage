#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

using String = std::string;
using Flag = uint32_t;
#define B_SIZE sizeof(Flag)

String descr_path = "descriptor.txt";

#define NEUTRAL 48
#define READ 49
#define WRITE 50

// The descriptor must be reseted each time a new server in launched
// We must keep track of running server instances (file with PID ?)

void create_descr(){
	
	std::ofstream str(descr_path, std::ios::out | std::ios::binary);
	Flag f = NEUTRAL;

	str.write((char*)&f, B_SIZE);

	str.close();
}


Flag inspect_flag(){
	Flag f = NEUTRAL;

	std::ifstream str(descr_path, std::ios::in | std::ios::binary);
	str.read((char*)&f, B_SIZE);
	str.close();

	return f;
}

void release(){
	Flag f = NEUTRAL;
	std::ofstream str(descr_path, std::ios::out | std::ios::binary);
	str.seekp(std::ios_base::beg);
	str.write((char*)&f, B_SIZE);
	str.close();
}


void write_lock(){
	Flag f = WRITE;
	std::ofstream str(descr_path, std::ios::out | std::ios::binary);
	str.seekp(std::ios_base::beg);
	str.write((char*)&f, B_SIZE);
	str.close();
}


void read_lock(){
	Flag f = READ;
	std::ofstream str(descr_path, std::ios::out | std::ios::binary);
	str.seekp(std::ios_base::beg);
	str.write((char*)&f, B_SIZE);
	str.close();
}


void pgm1(){
	int k = 0;
	while(true){
		k = rand() % 2;
		if (k){
			read_lock();
		}
		else{
			write_lock();
		}
		sleep(1);
		release();
		sleep(1);
	}
}


void pgm2(){
	while(true){
		switch(inspect_flag()){
			case(NEUTRAL):
				std::cout << "Free" << std::endl;
			break;
			case(READ):
				std::cout << "Reading" << std::endl;
			break;
			case(WRITE):
				std::cout << "Writting" << std::endl;
			break;
		}
		sleep(1);
	}
}


int main(int argc, char* argv[], char* env[]){

	srand(time(NULL));

	if (argv[1][0] == '0'){
		std::cout << "PGM1 launched" << std::endl;
		pgm1();
	}
	else{
		std::cout << "PGM2 launched" << std::endl;
		pgm2();
	}

	/*create_descr();
	std::cout << inspect_flag() << std::endl;

	write_lock();
	std::cout << inspect_flag() << std::endl;

	release();
	std::cout << inspect_flag() << std::endl;

	read_lock();
	std::cout << inspect_flag() << std::endl;

	release();
	std::cout << inspect_flag() << std::endl;*/

	return 0;
}