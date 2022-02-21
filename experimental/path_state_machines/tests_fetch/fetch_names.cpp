// Look for: "//../soussane-modules/lib_chars_main_soussane_rig_rig_with_modul.blend"
// File location: "/home/clement.benedetti/Bureau/tests-cpp-python/untitled.blend"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <chrono>

#define TYPE char

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR "\\" 
#else 
#define PATH_SEPARATOR "/" 
#endif 

#define SIZE_BUF_IFSTREAM 512

using luint = long unsigned int;

std::string w_dir = "/home/clement/Desktop/tests-cpp/"; // Working directory

class BufferedIfstream : public std::ifstream{
	size_t size_buffer = SIZE_BUF_IFSTREAM;
	char buffer[SIZE_BUF_IFSTREAM] = {0};

	size_t count = 0;
	size_t head = 0;

	luint pos = 0;
	bool next = true;

	private:

		bool load(){
			this->read(this->buffer, this->size_buffer);
			this->count = this->gcount();
			return !this->eof();
		}

	public:

		size_t size() const{
			return this->pos;
		}

		BufferedIfstream(std::string s) : std::ifstream(s){
			this->load();
		}

		luint tellgc() const{
			return this->pos;
		}

		inline bool get_char(char& c){
			if (this->head == this->count){
				if (!this->next){
					return false;
				}
				else{
					this->next = this->load();
					if (!this->count){ // If 0 char as been read from the file
						return false;
					}
					this->head = 0;
				}
			}
			this->pos++;
			c = this->buffer[this->head++];
			return true;
		}
};


void filter3(std::string path, std::string file, std::vector<luint>& indices){
	BufferedIfstream to_filter(path + file);
	char c = '\0';
	indices.clear();

	while (to_filter.get_char(c)){
		size_t i = 0;
		std::string found(w_dir.size(), 0);
		//found.reserve(w_dir.size());
		luint pos = to_filter.tellgc();

		while((i < w_dir.size()) && (c == w_dir[i])){
			found[i++] = c;
			if (!to_filter.get_char(c)){
				break;
			}
		}
		if(i == w_dir.size()){
			indices.push_back(pos-1);
		}
	}

	to_filter.close();
}


int main(int argc, char* argv[], char* env[]){

	std::string path = "/home/clement/Desktop/tests-cpp/";
	std::string file = "long_blend.blend";

	

	for(uint i = 0 ; i < 100 ; i++){

		auto start = std::chrono::high_resolution_clock::now();

		std::vector<luint> matches = {};
		filter3(path, file, matches);

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

		std::cout << duration.count() << std::endl;
	}
	

	return 0;
}

