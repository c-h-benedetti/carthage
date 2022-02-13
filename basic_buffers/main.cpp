#include <iostream>
#include "Name.hpp"
#include "SystemName.hpp"
#include "Buffer.hpp"

void testing_1(){
	const char* txt = "01234567890123456789";

	std::cout << " = STARTING =" << std::endl;

	StackSegment<8> b1{};
	StackSegment<8> b2{b1};
	StackSegment<8> b3{txt, 8};
	StackSegment<8> b4{txt, 12};
	StackSegment<8> b5 = b4;
	StackSegment<9> b6{};

	std::cout << " = PART 1 =" << std::endl;

	std::cout << b1.size() << std::endl;
	std::cout << b3.size() << std::endl;
	std::cout << b4.size() << std::endl;

	std::cout << " = PART 2 =" << std::endl;

	std::cout << b1.length() << std::endl;
	std::cout << b3.length() << std::endl;
	std::cout << b4.length() << std::endl;

	std::cout << " = PART 3 =" << std::endl;


	std::cout << " = END =" << std::endl;
}


void testing_2(){
	Name n1{};
	Name n2("some name");
	std::cout << "N1: " << n1 << std::endl;
	std::cout << "N2: " << n2 << std::endl;

	ID i1{};
	ID i2{1};
	std::cout << "ID1: " << i1 << std::endl;
	std::cout << "ID2: " << i2 << std::endl;

	Extension e1("blend");

	SystemName sn(i2, e1);
	std::cout << sn << std::endl;

	BasicBuffer<SIZE_OF_ID> bb{};

	bb.add(i2);
	
	std::cout << "Buffer: " << bb << std::endl;
}


int main(int argc, char* argv[], char* env[]){

	testing_2();

	return 0;
}