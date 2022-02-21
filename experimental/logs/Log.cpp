#include "Log.hpp"
#include <ctime>
#include <iostream>
#include <cstring>

Log* Log::instance = nullptr;

void write_file(){
	if (Log::instance != nullptr){
		std::cout << "=== Starting runner: Logs recorder ===" << std::endl;
		Log& l = Log::log();
		size_t writting = 0;

		do{
			if (!l.available[writting]){
				if (l.time_stamp){
					time_t t = time(NULL);
					char* ct = ctime(&t);
					for (std::ofstream& f : l.files){
						// f.write(ct, strlen(ct));
						f << ">>> " << ct;
					}
				}
				for (std::ofstream& f : l.files){
					// f.write(l.lines[writting], strlen(l.lines[writting]));
					f << l.lines[writting];
				}
				memset(l.lines[writting], 0, SIZE_BUFFER);
				l.available[writting] = true;
				writting = (writting == NB_BUFFERS - 1) ? (0) : (writting + 1);
			}
		}while(l.running || l.busy());
		std::cout << "=== Logs recorder runner terminated ===" << std::endl;
	}
	else{
		std::cerr << "=== Failed to launch logs recorder runner ===" << std::endl;
	}
}


Log::Log(){
	for (size_t i = 0 ; i < NB_BUFFERS ; i++){
		this->lines[i] = new char [SIZE_BUFFER];
		memset(this->lines[i], 0, SIZE_BUFFER);
		this->available[i] = true;
		this->n_line[i] = false;
	}
	this->running = true;
}


Log& Log::add(int a){
	size_t sum = (size_t)a + this->line_pos;
	if ((a > 0) && (sum <= SIZE_BUFFER)){
		this->line_pos = sum;
	}
	return *(this);
}


Log& Log::add(const char* s){
	size_t len = strlen(s);
	memcpy(this->buffer(), s, len);
	this->line_pos += len;
	return *(this);
}


Log& Log::write(){
	this->available[this->current] = false;
	size_t idx = this->next();
	while (!this->available[idx]){
		// Try to avoid potential infinite loop?
		// std::cout << "- BUFFERING -" << std::endl;
	}
	this->current = idx;
	this->line_pos = 0;
	return *(this);
}


Log& Log::log(){
	if (Log::instance == nullptr){
		Log::instance = new Log{};
		Log::instance->runner = std::thread(write_file);
	}
	return *(Log::instance);
}


void Log::add_stream(const std::string& path){
	Log& l = Log::log();

	if (path.size() > 0){
		l.files.push_back(std::ofstream(path));
	}
	else{
		l.files.push_back(std::ofstream("log.log"));
	}
}

Log::~Log(){
	this->running = false;
	this->runner.join();
	for (std::ofstream& f : this->files){
		f.close();
	}
	for (size_t i = 0 ; i < NB_BUFFERS ; i++){
		delete[] this->lines[i];
	}
}

int Log::terminate(){
	if (Log::instance != nullptr){
		delete Log::instance;
		return 0;
	}
	return -1;
}
