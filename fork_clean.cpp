#include <iostream>
#include <unistd.h>
#include "Subprocess.hpp"

int main(int argc, char* argv[], char* env[]){

  std::function<void(Subprocess::State)> f1 = [=](Subprocess::State s){
    std::cout << "State: " << (int)s << std::endl;
  };

  Subprocess s{
    "blender", // Command from the PATH
    {
      "--python-expr", // This argument and the next have for purpose to disable python buffering on stdout to get instantly the results of print()
      "import sys\ntry:\n    sys.stdout.reconfigure(line_buffering=True)\nexcept TypeError:\n    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)"
    },
    "blend_log.log", // Name of log file
    "blender" // Name of process in terminal (purely aesthetic)
  };

  s.subscribe_to(f1); // Registering a callback function
  s.launch();

  // Without while, the parent process reaches the end
  // In a real software, we suppose that we are in the main execution loop

  while(s.get_state() == Subprocess::State::RUNNING){
    std::cout << "RUNNING" << std::endl;
    sleep(1);
  }

  std::cout << "END" << std::endl;

  return 0;
}

