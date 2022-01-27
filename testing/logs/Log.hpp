#ifndef LOG_HANDLER_HPP_INCLUDED
#define LOG_HANDLER_HPP_INCLUDED

#include <fstream>
#include <vector>
#include <thread>

#define SIZE_BUFFER 128
#define NB_BUFFERS 25

class Log{

	static Log* instance;

	std::vector<std::ofstream> files{};

	char* lines[NB_BUFFERS] = {nullptr};
	bool  n_line[NB_BUFFERS] = {false};
	bool  available[NB_BUFFERS] = {true};

	size_t line_pos = 0;
	size_t current = 0;
	bool   running = false;

	std::thread runner;

	inline bool busy() const{
		for(size_t i = 0 ; i < NB_BUFFERS ; i++){
			if (!this->available[i]){
				return true;
			}
		}
		return false;
	}

	inline size_t next(){return (this->current == NB_BUFFERS - 1) ? (0) : (this->current + 1);}

	Log();
	Log(const Log& l) = delete;
	Log(Log&& l) = delete;
	Log& operator=(const Log& l) = delete;
	Log& operator=(Log&& l) = delete;


public:

	bool time_stamp = false;

	Log& add(int a);
	Log& add(const char* s);
	Log& write();
	inline char* buffer(){return this->lines[this->current] + this->line_pos;}
	inline size_t buffer_size() const{return SIZE_BUFFER;}	

	static Log& log();
	static int terminate();
	static void add_stream(const std::string& path="");

	~Log();

	friend void write_file();

};

#endif // LOG_HANDLER_HPP_INCLUDED

/*

[X] Move logs on another thread to keep the CPU free
[ ] Testing session
[ ] Add an alternative method to output log as an HTML file (provide CSS ?)
[ ] Try to see if there is an easier way to get formatted strings
[ ] Find a way to add streams to that system, such as cerr (or option disp_in_terminal ?)
[ ] Make a Windows version (std::thread in Unix-only)

*/