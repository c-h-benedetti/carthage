#ifndef BASIC_CSV_IO_HPP_INCLUDED
#define BASIC_CSV_IO_HPP_INCLUDED

#include <string>
#include <fstream>
#include <vector>

extern std::string env_path;
extern size_t def_size;
extern char separator;

class CSV_Content{
public:
	std::vector<std::vector<std::string>> lines;
};

class BasicCSV_Reader{
private:
	std::ifstream input;
	size_t fixed_length = 0;

	bool open(const std::string& s=env_path);
	void close();

public:
	CSV_Content read{};

	BasicCSV_Reader();
	BasicCSV_Reader(const std::string& path, size_t fixed_len=0);
	~BasicCSV_Reader();
	
	void read_all();

};

class BasicCSV_Writter{
private:
	std::ofstream output;

public:
	CSV_Content write{};

	bool open();
	bool dump();
	void add_line(const std::string& s);
	void add_line(std::string&& s);
	void write_line(const std::string& s);
	void close();

};

#endif //BASIC_CSV_IO_HPP_INCLUDED