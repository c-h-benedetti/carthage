#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <sys/stat.h>
#include <string>
#include <vector>

#if defined(WIN32) || defined(_WIN32) 
	#define PATH_SEPARATOR "\\" 
#else 
	#define PATH_SEPARATOR "/" 
#endif 

using uint = unsigned int;
#define PIPE_INPUT 1
#define PIPE_OUTPUT 0
#define forever for(;;)

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// ~                                                                         ~
// ~  Pool of functions that looks for an exec path through environment      ~
// ~                                                                         ~
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

// Check if a file exists ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
inline bool exists (const std::string& name);

// Checks if a string has a precise prefix ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
bool starts_with(const std::string& desired, const std::string& pool);

// Try to fetch an environment variable from env ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
void get_env_variable(std::string&& name, char* env[], std::vector<std::string>& values);

// Try to fetch exec path from list of paths ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 
void find_exe_paths(std::vector<std::string>& found, std::string& command, char* env[]);

#endif //UTILS_HPP_INCLUDED