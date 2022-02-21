#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <chrono>
#include <queue>
#include <mutex>

// Size must be large enough to hold the algorithm as much time as reading the file
#define forever while(true)

using luint = long unsigned int;
using lint = long int;

luint SIZE = 65536*2; // Pourri les temps de façon exponentielle en diminuant

char w_dir[] = "/home/clement/Desktop/tests-cpp/";

std::queue<char*>  buffers{};
std::queue<size_t> size_buffers{};
std::mutex buffers_mutex;

bool done = false;
size_t limit_size = 32; // Améliore légèrement les temps en augmentant

void read_file(const char* filepath){
	std::ifstream input(filepath);
	
	while (!input.eof()){
		if (buffers.size() <= limit_size){
			char* buffer = new char[SIZE];
			input.read(buffer, SIZE);
			std::lock_guard<std::mutex> guard(buffers_mutex);
			buffers.push(buffer);
			size_buffers.push(input.gcount());
		}
	}
	done = true;
	input.close();
}


// Returns -1 if nothing found, else the position in w_dir to the one we stopped
// If we have several occurences, the index in the last occurence is provided
void parse_phrase(char phrase[], size_t size, luint* start){
	luint found = *start;
	char c1 = 0, c2 = 0;
	size_t w_dir_s = sizeof(w_dir) - 1;

	for (luint i = 0 ; i < size ; i++){
		while ((i < size) && (found < w_dir_s) && ((c1 = phrase[i]) == (c2 = w_dir[found]))){
			i++;
			found++;
		}
		if (found == w_dir_s){ // -1 to exclude the '\0'
			// What to do in case of match
			// std::cout << "MATCH!" << std::endl;
		}
		if ((found >= w_dir_s) or (c1 != c2)){
			found = 0;
		}
	}

	*start = found;
	delete[] phrase;
}

size_t inspect_file(const std::string filepath){
	std::thread t1(read_file, filepath.c_str());
	luint start = 0;

	char* to_read = nullptr; // Move outside the function to improve time by moving the call to parse_phrase before the lock_guard
	size_t size = 0;

	forever{

		if(!buffers.empty()){
			std::lock_guard<std::mutex> guard(buffers_mutex);
			to_read = buffers.front();
			size = size_buffers.front();
			buffers.pop();
			size_buffers.pop();
		}
		if (to_read != nullptr){
			parse_phrase(to_read, size, &start);
			to_read = nullptr;
		}

		if (done && buffers.empty()){
			break;
		}
	}

	t1.join();
	return 0;
}


int main(int argc, char* argv[], char* env[]){

	std::string path = "/home/clement/Desktop/tests-cpp/";
	std::string file = "long_blend.blend";

	for(uint i = 0 ; i < 200 ; i++){
		buffers = std::queue<char*>{};
		size_buffers = std::queue<size_t>{};
		done = false;
		auto start = std::chrono::high_resolution_clock::now();

		inspect_file(path + file);

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		std::cout << duration.count() << std::endl;
	}

	return 0;
}