#include <iostream>
#include <vector>
#include <cstring>

using Vector = std::vector<int>;
using VecIt = std::vector<int>::iterator;

void tell(const Vector& v){
	for (const int& i : v){
		std::cout << i << "  ";
	}std::cout << std::endl;
}

int main(int argc, char* argv[], char* env[]){

	Vector v = {1, 2, 3, 4, 5, 6};
	
	Vector v2;
	v2.resize(6);

	memcpy(&(v2.at(0)), &(v.at(0)), 6 * sizeof(int));

	tell(v);
	tell(v2);
	
}