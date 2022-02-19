#include <iostream>
#include <vector>
#include <cstring>

int main(int argc, char* argv[], char* env[]){

	std::vector<int> v1 = {1, 2, 3, 4, 5, 6};

	v1.resize(12);

	int t1[] = {1, 2, 3, 4, 5, 6};

	memcpy(v1.data()+6, t1, 6*sizeof(int));

	std::cout << "=== " << v1.capacity() << " ===" << std::endl;

	for(const int& i : v1){
		std::cout << i << std::endl;
	}

	return 0;
}