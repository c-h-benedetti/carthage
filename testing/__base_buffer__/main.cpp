#include <iostream>
#include "StackSegment.hpp"

int main(int argc, char* argv[], char* env[]){

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

	std::cout << b1.max() << std::endl;
	std::cout << b3.max() << std::endl;
	std::cout << b4.max() << std::endl;

	std::cout << " = PART 3 =" << std::endl;

	std::cout << b1.is_full() << std::endl;
	std::cout << b3.is_full() << std::endl;
	std::cout << b4.is_full() << std::endl;

	std::cout << " = PART 4 =" << std::endl;

	/*std::cout << b1.add(txt, 12) << std::endl;
	std::cout << b3.add(txt, 12) << std::endl;
	std::cout << b4.add(txt, 12) << std::endl;*/

	std::cout << " = PART 5 =" << std::endl;

	char c1[9] = {};
	char c2[9] = {};
	char c3[9] = {};

	b1.copy_all(c1);
	b3.copy_all(c2);
	b4.copy_all(c3);

	std::cout << c1 << std::endl;
	std::cout << c2 << std::endl;
	std::cout << c3 << std::endl;

	std::cout << " = PART 6 =" << std::endl;

	b1.write_all_to(std::cout);
	std::cout << std::endl;
	b3.write_all_to(std::cout);
	std::cout << std::endl;
	b4.write_all_to(std::cout);
	std::cout << std::endl;

	b1.write_to(std::cout);
	std::cout << std::endl;
	b3.write_to(std::cout);
	std::cout << std::endl;
	b4.write_to(std::cout);
	std::cout << std::endl;

	std::cout << " = PART 7 =" << std::endl;


	StackSegment<8> b9{};
	b9.add<char>(48);
	b9.write_to(std::cout);


	std::cout << " = END =" << std::endl;

	return 0;
}