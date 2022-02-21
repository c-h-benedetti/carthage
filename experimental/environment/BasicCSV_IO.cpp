#include "BasicCSV_IO.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>

std::string env_path = "env.csv";
size_t def_size = 512;
const std::string WHITESPACE = " \n\r\t\f\v";
char separator = ',';

bool BasicCSV_Reader::open(const std::string& s){
	this->input = std::ifstream(s);
	return this->input.is_open();
}

void BasicCSV_Reader::close(){
	this->input.close();
}

void BasicCSV_Reader::read_all(){

	char buffer[def_size] = {0};

	while (this->input.getline(buffer, def_size)){
		std::string res(buffer);

		if (res.size() == 0){continue;}
		
		std::string token = "";
		size_t last = 0;
		std::vector<std::string> line{};

		do{
			size_t pos = res.find(separator, last);
			token = res.substr(last, pos-last);

			size_t start = token.find_first_not_of(WHITESPACE);
			size_t end = token.find_last_not_of(WHITESPACE);
			token = token.substr(start, end-start+1);

			line.push_back(token);
			last = pos + 1; // (std::string::posn + 1) == 0

		}while(last > 0);


		if (this->fixed_length == 0){
			this->read.lines.push_back(line);
		}
		else if (this->fixed_length == line.size()){
			this->read.lines.push_back(line);
		}
		memset(buffer, 0, def_size);
	}
}

BasicCSV_Reader::BasicCSV_Reader(){
	if (!this->open()){
		throw 20;
	}
	this->read_all();
}

BasicCSV_Reader::BasicCSV_Reader(const std::string& path, size_t fixed_len){
	if (!this->open(path)){
		throw 20;
	}
	this->fixed_length = fixed_len;
	this->read_all();
}

BasicCSV_Reader::~BasicCSV_Reader(){
	this->close();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

bool BasicCSV_Writter::open(){
	return false;
}

bool BasicCSV_Writter::dump(){
	return false;
}

void BasicCSV_Writter::add_line(const std::string& s){

}

void BasicCSV_Writter::add_line(std::string&& s){

}

void BasicCSV_Writter::write_line(const std::string& s){

}

void BasicCSV_Writter::close(){

}