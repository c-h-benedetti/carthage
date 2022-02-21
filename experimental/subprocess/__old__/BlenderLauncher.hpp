#include "Subprocess.hpp"

// We must include a way to specify a version

class BlenderLauncher : public Subprocess{

  protected:

    std::string open_file;

  public:

    BlenderLauncher(
      std::string file="",
      std::string log_path="last_blend.log",
      std::string p_name="Blender"
    );

  	BlenderLauncher(
      std::vector<std::string> args,
      std::string log_path="last_blend.log",
      std::string p_name="Blender"
    );
};