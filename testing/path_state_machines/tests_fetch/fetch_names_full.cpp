// Look for:
// "//../soussane-modules/lib_chars_main_soussane_rig_rig_with_modul.blend"
// File location:
// "/home/clement.benedetti/Bureau/tests-cpp-python/untitled.blend"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <vector>
#include <chrono>
#include <regex>
#include <streambuf>
#include <stdio.h>
#include <memory>

#define TYPE char

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define SIZE_BUF_IFSTREAM 2048

using luint = long unsigned int;

std::string w_dir = "/home/clement/Desktop/tests-cpp/";  // Working directory

std::string read_file(std::string file_name) {
  // opening the file in read mode
  FILE* fp = fopen(file_name.c_str(), "r");

  // checking if the file exist or not
  if (fp == NULL) {
    printf("File Not Found!\n");
    return nullptr;
  }

  fseek(fp, 0L, SEEK_END);

  // calculating the size of the file
  luint res = ftell(fp);

  // char* buffer = new char [res];
  std::string s(res, '\0');

  fseek(fp, 0L, SEEK_SET);

  fread(&s[0], 1, res, fp);

  // closing the file
  fclose(fp);

  return s;
}

int main(int argc, char* argv[], char* env[]) {
  std::string path = "/home/clement/Desktop/tests-cpp/";
  std::string file = "long_blend.blend";

  // Reading the file takes a crazy time
  // Since we are physically limited by the disk, we can't get faster
  // See if it's not possible to be fast with a more C like method
  // This way to proceed gives a worst result than the worse result to copy the
  // file with the buffered ifstream
  for (uint i = 0; i < 100; i++) {
    auto start = std::chrono::high_resolution_clock::now();

    /*std::ifstream t(path + file);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();*/
            

    std::string str(read_file(path + file));

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << duration.count() << std::endl;
  }
  /*
      std::regex re1(w_dir);
      std::match_results<std::string::iterator> m;
  //bool success = std::regex_search(str, m, re1);
  bool success = std::regex_search(str.begin(), str.end(), m, re1);

      if (success){
              std::cout << m[0] << std::endl;
              size_t after = m.position() + m.length();
              std::cout << after << std::endl;
              // Position of match is relative to the start of the sequence
              // To iterate, we must add after and the new after
              success = std::regex_search(str.begin()+after, str.end(), m, re1);
              if (success){
                      std::cout << m[0] << std::endl;
                      after = m.position() + m.length();
                      std::cout << after << std::endl;
              }
      }
      */

  // Segment the string in several pieces and launch a thread on each of these
  // piece, with an overlap of the same
  // size as the working_directory.
  // The vector storing results would require a mutex in this case

  return 0;
}
