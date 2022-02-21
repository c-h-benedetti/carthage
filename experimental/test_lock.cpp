#include <iostream>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <chrono>

namespace fs = std::filesystem;

class Manager{

	std::mutex file_mutex;
	fs::path file_path = "testing.txt";
	char c = 'A';

public:

	void reading_operation(){
		const uint sz = 1024;
		char buffer[sz] = {};

		std::ifstream stream(this->file_path.c_str(), std::ios::binary | std::ios::in);
		stream.read(buffer, sz-1);
		stream.close();

		std::cout << buffer << std::endl;
	}

	void writting_operation(){
		std::lock_guard<std::mutex> guard(this->file_mutex);
		
		std::ofstream stream(this->file_path.c_str(), std::ios::binary | std::ios::out | std::ios_base::app);
		stream.write(&this->c, 1);
		stream.close();

		this->c++;
	}
};


void function(Manager* m){
	srand(time(0));
	for (uint i = 0 ; i < 10 ; i++){
		int h_or_t = rand() % 10;
		if (h_or_t < 3){
			m->writting_operation();
		}
		else{
			m->reading_operation();
		}
		int sleeping = rand() % 20;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleeping));
	}
}


int main(){

	Manager m{};

	std::thread t1{function, &m};
	std::thread t2{function, &m};
	std::thread t3{function, &m};
	std::thread t4{function, &m};
	std::thread t5{function, &m};
	std::thread t6{function, &m};
	std::thread t7{function, &m};
	std::thread t8{function, &m};

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();

	return 0;
}


/*

It doesn't matter how many threads are reading at the same time from the file, but only one thread must write the file at the time.

*/