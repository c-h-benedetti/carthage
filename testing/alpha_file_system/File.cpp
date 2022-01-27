#include "File.hpp"
#include <cctype>
#include <iostream>
#include <fstream>

File::File() : FileSystemObject(){

}

File::File(const std::string& s) : FileSystemObject(s){

}

File::File(const std::string& s, const fs::directory_entry& d) : File(s){
	this->determine_type(d);
}

int File::launch(){
	return (int)File::LaunchStatus::UNKNOWN;
}

void File::determine_type(const fs::directory_entry& d){
	if (d.exists()){
		if (d.path().has_extension()){
			std::string s = d.path().extension();

			for (char& c : s){
				c = std::tolower(c);
			}

			if (s == ".blend"){this->type = Type::BLEND;}
			else if (s == ".blend1"){this->type = Type::BLEND;}
			else if (s == ".txt"){this->type = Type::TXT;}
			else if (s == ".odt"){this->type = Type::ODT;}
			else if (s == ".html"){this->type = Type::HTML;}
			else if (s == ".cpp"){this->type = Type::CPP;}
			else if (s == ".hpp"){this->type = Type::HPP;}
			else if (s == ".c"){this->type = Type::C;}
			else if (s == ".h"){this->type = Type::H;}
			else if (s == ".zip"){this->type = Type::ZIP;}
			else{this->type = Type::UNKNOWN;}
		}
		else{
			// No extension = either binary (linux) or unknown
			uint32_t buffer = 0; // 4 bytes
			std::ifstream f(d.path());
			f.read((char*)&buffer, 4);
			f.close();

			if (buffer == UNIX_BIN){
				this->type = Type::UNIX_BINARY;
			}
			else{
				this->type = Type::UNKNOWN;
			}
		}
	}
}

File::Type File::get_type() const{
	return this->type;
}
