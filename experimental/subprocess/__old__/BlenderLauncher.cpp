#include "BlenderLauncher.hpp"

BlenderLauncher::BlenderLauncher(std::string file, std::string log_path, std::string p_name) : Subprocess(log_path, p_name){
	this->command = "blender";
	this->_command_ = "/media/clement/Softwares/blender-2.92.0-linux64/blender";
	if (file.size() > 0){
		this->command_line_args = {
			file.c_str(),
			"--python-expr",
			"import sys\ntry:\n    sys.stdout.reconfigure(line_buffering=True)\nexcept TypeError:\n    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)",
		};
	}
	else{
		this->command_line_args = {
			"--python-expr",
			"import sys\ntry:\n    sys.stdout.reconfigure(line_buffering=True)\nexcept TypeError:\n    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)",
		};
	}
}

BlenderLauncher::BlenderLauncher(std::vector<std::string> args, std::string log_path, std::string p_name) : Subprocess(args, log_path, p_name){
	this->command = "blender";
	this->_command_ = "/media/clement/Softwares/blender-2.92.0-linux64/blender";
}