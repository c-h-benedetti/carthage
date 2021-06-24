#include <iostream>
#include <unistd.h>
#include "Subprocess.hpp"

class SomeObject{
  int some_attribute = 0;

  public:

    std::function<void(Subprocess::State)> f1 = [=](Subprocess::State s){
      this->process_signal(s);
    };

    void process_signal(Subprocess::State state){
      switch(state){
        case(Subprocess::State::RUNNING):
          this->some_attribute = 10;
          break;

        case(Subprocess::State::FINISHED):
          this->some_attribute = 1;
          break;

        case(Subprocess::State::KILLED):
          this->some_attribute = 20;
          break;

        default:
          break;
      };
    }

};

int main(int argc, char* argv[], char* env[]){

  Subprocess s{
    "blender", // Command from the PATH
    {
      "/home/clement.benedetti/Bureau/empty.blend",
      "--python-expr", // This argument and the next have for purpose to disable python buffering on stdout to get instantly the results of print()
      "import sys\ntry:\n    sys.stdout.reconfigure(line_buffering=True)\nexcept TypeError:\n    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)"
    },
    "blend_log.log", // Name of log file
    "blender" // Name of process in terminal (purely aesthetic)
  };

  SomeObject obj{};
  s.subscribe_to(obj.f1); // Registering a callback function
  s.launch();

  // Without while, the parent process reaches the end
  // In a real software, we suppose that we are in the main execution loop
  
  while(s.get_state() == Subprocess::State::RUNNING){
    //std::cout << "RUNNING" << std::endl;
    sleep(1);
  }

  return 0;
}

