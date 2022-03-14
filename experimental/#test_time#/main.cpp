#include <iostream>
#include <filesystem>
#include <cstring>
#include <chrono>

namespace fs = std::filesystem;

int main(int argc, char* argv[], char* env[]){

	fs::file_time_type time = fs::last_write_time("/home/clement/Desktop/carthage/experimental/test_time/main.cpp");
	std::cout << "Size: " << sizeof(fs::file_time_type) << std::endl;

	uint64_t k = 0;
	memcpy(&k, &time, 8);
	std::cout << "Time: " << k << std::endl;

	uint64_t u1 = 13655838435299788648;
	uint64_t u2 = 13655838443047463601;

	fs::file_time_type t1;
	fs::file_time_type t2;

	memcpy((uint64_t*)&t1, &u1, 8);
	memcpy((uint64_t*)&t2, &u2, 8);

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);
	std::cout << duration.count() << std::endl;

	return 0;
}

// 13655838435299788648
// 13655838443047463601