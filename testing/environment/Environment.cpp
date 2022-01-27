#include "Environment.hpp"
#include <regex>
#include <cstring>

std::regex var_name_expr("([a-zA-Z0-9_]+)");

std::regex int_expr("-?[1-9]+[0-9]*", std::regex_constants::ECMAScript);
std::regex float_expr("[+-]?([0-9]*[.])?[0-9]+", std::regex_constants::ECMAScript);
std::regex char_expr("([\x01-\x7F])", std::regex_constants::ECMAScript);
std::regex string_expr("(([\x01-\x7F])*)", std::regex_constants::ECMAScript);

EnvType detected_type(const std::string& s){

	if (s == "INTEGER"){
		return EnvType::INTEGER;
	}
	if (s == "CHARACTER"){
		return EnvType::CHARACTER;
	}
	if (s == "FLOAT"){
		return EnvType::FLOATING;
	}
	if (s == "STRING"){
		return EnvType::STRING;
	}

	return EnvType::UNKNOWN;
}

Environment::Environment(){
	std::cout << "Building environment variables" << std::endl;
	BasicCSV_Reader rd(env_path, 3);
	this->parse(rd);
	std::cout << "Environment variables built" << std::endl;
}

Environment::Environment(const std::string& s){
	std::cout << "Building environment variables" << std::endl;
	env_path = s;
	BasicCSV_Reader rd(env_path, 3);
	this->parse(rd);
	std::cout << "Environment variables built" << std::endl;
}

bool Environment::save(){
	return false;
}

void Environment::tell_environment(std::ostream& o) const{
	o << " === INTEGERS === (" << this->integers.size() << ") " << std::endl;
	for (std::map<std::string, int>::const_iterator it = this->integers.begin() ; it != this->integers.end() ; it++){
		o << "$(" << it->first << "): " << it->second << std::endl;
	}
	o << std::endl;
	o << " === FLOATS === (" << this->floats.size() << ") " << std::endl;
	for (std::map<std::string, float>::const_iterator it = this->floats.begin() ; it != this->floats.end() ; it++){
		o << "$(" << it->first << "): " << it->second << std::endl;
	}
	o << std::endl;
	o << " === CHARS === (" << this->chars.size() << ") " << std::endl;
	for (std::map<std::string, char>::const_iterator it = this->chars.begin() ; it != this->chars.end() ; it++){
		o << "$(" << it->first << "): " << it->second << std::endl;
	}
	o << std::endl;
	o << " === STRINGS === (" << this->strings.size() << ") " << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->strings.begin() ; it != this->strings.end() ; it++){
		o << "$(" << it->first << "): " << it->second << std::endl;
	}
	o << std::endl;
}

void Environment::parse(BasicCSV_Reader& rd){
	std::vector<std::string> accepted{};

	for (const std::vector<std::string>& line : rd.read.lines){
		// We specified to the reader that we are only interested in lines with a length of 3 words
		std::cmatch m;
		if (std::regex_match (line[0].c_str(), m, var_name_expr)){ // Valid variable name
			std::string var_name = m[1];

			if (std::find(accepted.begin(), accepted.end(), var_name) != accepted.end()){
				std::cerr << "Variable: $(" << var_name << ") has already been recorded" << std::endl;
				continue;
			}
			
			EnvType data_type = detected_type(line[1]);
			if (data_type != EnvType::UNKNOWN){ // The data type is correct
				std::cmatch data_match;

				if (data_type == EnvType::INTEGER){
					if (std::regex_match (line[2].c_str(), data_match, int_expr)){
						this->integers[var_name] = atoi(line[2].c_str());
						accepted.push_back(var_name);
					}
					else{
						std::cerr << "Unrecognized value: " << line[2] << " as [int]" << std::endl;
					}
				}
				else if (data_type == EnvType::CHARACTER){
					if (std::regex_match (line[2].c_str(), data_match, char_expr)){
						std::string res = data_match[1];
						this->chars[var_name] = res[0];
						accepted.push_back(var_name);
					}
					else{
						std::cerr << "Unrecognized value: " << line[2] << " as [char]" << std::endl;
					}
				}
				else if (data_type == EnvType::STRING){
					if (std::regex_match (line[2].c_str(), data_match, string_expr)){
						std::string res = data_match[1];
						this->strings[var_name] = res;
						accepted.push_back(var_name);
					}
					else{
						std::cerr << "Unrecognized value: " << line[2] << " as [string]" << std::endl;
					}
				}
				else if (data_type == EnvType::FLOATING){
					if (std::regex_match (line[2].c_str(), data_match, float_expr)){
						this->floats[var_name] = atof(line[2].c_str());
						accepted.push_back(var_name);
					}
					else{
						std::cerr << "Unrecognized value: " << line[2] << " as [float]" << std::endl;
					}
				}
				else{
					std::cerr << "Type recognized but not handled for: " << line[1] << std::endl;
				}

			}
		}
	}


}