#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include "utils.hpp"

// Clement H. Benedetti (2021)
// Generic class to launch subprocess on a UNIX machine
// It is strongly recommended to specialize that class to launch a programm or another
// rather than passing values to the constructor.
// If you want to see the outputs of your application as fast as they are geerated, you
// must be sure that the buffers are not in use.
// ex: If you launch Python, pass "-u" as command line argument
// For now, it's not possible to collect the output of applications that automatically detach themselves (ex: firefox)
// A Subprocess works like a state machine so we can easily track what it is doing or if we can use it.
// With the function "subscribe_to", callbacks can be generated from everywhere.
// The States are explicitely named

enum PStatus{
  SUCCESS=0,
  FAILED_OVERLAY=(1<<0),
  FAILED_PIPE=(1<<1),
  FAILED_FORK=(1<<2),
};

class Subprocess{

  public:

    enum State{
      INVALID=0,
      INITIALIZED=(1<<0),
      RUNNING=(1<<1),
      FINISHED=(1<<2),
      DESTROYED=(1<<3)
    };

  protected:

    std::string command = "echo"; // Command to execute
    std::string _command_ = "echo"; // Command launched if command not found
    std::vector<std::string> command_line_args; // Arguments passed to the command line
    State state = State::INVALID;
    std::vector<std::function<void(State)>> subscribers = {}; // Even better than on youtube!

    std::string log_file_path;  // Path to log file
    std::string process_name;   // Displayed name in terminal output (among all other process)
    std::ofstream log_file;     // Stream to log file

    pid_t pid = 0;                    // pid of the subprocess
    int   file_descriptors[2] = {0};  // File descrpitors to communicate with subprocess
    int   return_status = -1;         // Status returned by the subprocess (-1 if not launched or not finished)

    std::thread thread_is_alive;  // Thread checking if the subprocess is still running
    std::thread thread_listener;  // Thread listening the stdout and stderr of subprocess
    
  protected:

    int  thread_is_done();      // Launch the thread that waits for the subprocess to die
    int  thread_listen_pipe();  // Launch thread looping of the pipe from the subprocess
    int  overlay();             // Overlay the forked process by the desired executable
    void secure_log_path();     // Checks that the log file doesn't have an empty path
    void is_over();             // Cleans up most of the stuff after the subprocess was finished
    void set_state(State s);    // Set the state and triggers registered callbacks

  public:

    State   get_state() const;  // Returns the exit status of the subprocess (-1 if still running or not launched)
    int     get_return_status() const;
    int     launch();     // Launches the subprocess with the current settings
    void    terminate();  // Kills the subprocess
    void    subscribe_to(const std::function<void(State)>& f);

    Subprocess(
      std::string cmd="echo",
      std::vector<std::string> args={},
      std::string log_path="last_log.log",
      std::string p_name="Blank Launcher"
    );

    ~Subprocess();

    friend void wait_for_subprocess(Subprocess* s, pid_t pid);
    friend void check_for_message(int fd_read, std::ofstream* o, std::string* command);
    friend void listen_to_pipe(Subprocess* s, int fd_read, std::ofstream* o, std::string* command);
};

void wait_for_subprocess(Subprocess* s, pid_t pid);
void check_for_message(int fd_read, std::ofstream* o, std::string* command);
void listen_to_pipe(Subprocess* s, int fd_read, std::ofstream* o, std::string* command);
