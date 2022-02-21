#include "utils.hpp"

// Check if a file exists ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

inline bool exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

// Checks if a string has a precise prefix ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

bool starts_with(const std::string& desired, const std::string& pool){
  return !pool.compare(0, desired.size(), desired);
}

// Try to fetch an environment variable from env ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

void get_env_variable(std::string&& name, char* env[], std::vector<std::string>& values){
  uint i = 0;
  values.clear();
  std::string found = "";

  while (env && env[i]){
    if (starts_with(name, env[i])){
      found = env[i];
    }
    i++;
  }
  if (found.size() == 0){
    return;
  }

  found = found.substr(found.find("=")+1, std::string::npos);
  
  size_t prev_pos = 0;
  size_t index = 0;

  while(index != std::string::npos){
    index = found.find(":", prev_pos);
    std::string chunk = found.substr(prev_pos, index - prev_pos);
    values.push_back(chunk);
    prev_pos = index + 1;
  }

}

// Try to fetch exec path from list of paths ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ 

void find_exe_paths(std::vector<std::string>& found, std::string& command, char* env[]){
  std::vector<std::string> path_item;
  path_item.clear();
  found.clear();
  get_env_variable("PATH", env, path_item);

  for (const std::string& s : path_item){
    if (exists(s + PATH_SEPARATOR + command)){
      found.push_back(s + PATH_SEPARATOR + command);
    }
  }
}