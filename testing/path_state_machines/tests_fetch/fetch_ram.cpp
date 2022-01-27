#include <iostream>
#include <string>
#include <thread>
#include <fstream>

#define SIZE 262144
//#define SIZE 5

using luint = long unsigned int;
using lint = long int;
char w_dir[] = "/home/clement/Desktop/tests-cpp/"; // '\0' counted by sizeof()
//char w_dir[] = "abcde";

// Returns -1 if nothing found, else the position in w_dir to the one we stopped
// If we have several occurences, the index in the last occurence is provided
void parse_phrase(char phrase[], size_t size, luint* start){
	luint found = *start;
	char c1 = 0, c2 = 0;

	for (luint i = 0 ; i < size ; i++){
		while ((i < size) && (found < sizeof(w_dir)-1) && ((c1 = phrase[i]) == (c2 = w_dir[found]))){
			i++;
			found++;
		}
		if (found == sizeof(w_dir)-1){ // -1 to exclude the '\0'
			// What to do in case of match
			//std::cout << "MATCH!" << std::endl;
		}
		if ((found >= sizeof(w_dir)-1) or (c1 != c2)){
			found = 0;
		}
	}

	*start = found;
}

size_t inspect_file(const std::string& filepath){
	char buffer_1[SIZE] = {0};
	char buffer_2[SIZE] = {0};
	bool switch_buffer  = false;
	luint position      = 0;

	std::ifstream file(filepath);
	std::thread last_thread;
	size_t total = 0, iterations = 0;

	while (!file.eof()){
		file.read((switch_buffer) ? (buffer_1) : (buffer_2), SIZE);

		if (last_thread.joinable()){
			last_thread.join();
		}


		last_thread = std::thread(parse_phrase, (switch_buffer) ? (buffer_1) : (buffer_2), file.gcount(), &position);
		switch_buffer = !switch_buffer;
		total += file.gcount();
		iterations++;
	}

	last_thread.join();
	file.close();

	return 0;
}


int main(int argc, char* argv[], char* env[]){

	std::string path = "/home/clement/Desktop/tests-cpp/";
	std::string file = "long_blend.blend";

	for(uint i = 0 ; i < 100 ; i++){
		auto start = std::chrono::high_resolution_clock::now();

		inspect_file(path + file);

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

		std::cout << duration.count() << std::endl;
	}

	return 0;
}