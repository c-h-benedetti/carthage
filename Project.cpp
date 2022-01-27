#include "Project.hpp"
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iostream>

#define PROJECTS_FILE "data/projects_list.txt"

/*

 => Folder of a Carthage project

 ─ $(project_name)/
    ├─ <".carthage/">
    │   └─ $("vfs_descriptor.ctg")
    ├─ $(project_name)/
    │   ├─ "fs_obj_1"
    │   ├─ "fs_obj_2"
    │   └─ "fs_obj_3"

*/

/*

=> Carthage installation folder

 ─ "some_name/"
    ├─ <"data/">
    │   └─ <"projects_list.txt">


[!]: The <"abc"> notation indicates that this value is a constant (preprocessor || static segment)
[!]: The $(var) notation indicates that this value is provided by the user

*/

int Project::init_project() const{

	char folder_name[sizeof(Name)+1] = {};
	this->name.valid(folder_name);

	Path carthage_dir = this->path / CARTHAGE_DIR; // 
	Path descriptor = carthage_dir / VFS_DESCRIPTOR;
	Path data_dir = this->path / folder_name;

	if (fs::exists(carthage_dir)){
		return 1;
	}
	else{
		if (!fs::create_directory(data_dir)){
			return 4;
		}
		if (fs::create_directory(carthage_dir)){
			std::ofstream stream(descriptor.c_str(), std::ios::binary | std::ios::out);
			if (stream.is_open()){
				FSBlock block{};
				block.content_address = 0;
				block.name = this->name;
				block.type = FSType::ROOT;
				block.last_edition_date = fs::last_write_time(this->path);

				FSize size = 1;
				FPos end = 0;

				char buffer[sizeof(size) + sizeof(FSBlock) + sizeof(end)] = {};
				
				memcpy(buffer, &size, sizeof(FSize));
				memcpy(buffer + sizeof(FSize), &block, sizeof(FSBlock));
				memcpy(buffer + sizeof(FSize) + sizeof(FSBlock), &end, sizeof(FPos));

				stream.write(buffer, sizeof(size) + sizeof(FSBlock) + sizeof(end));

				stream.close();

				return 0;
			}
			else{
				return 3;
			}
		}
		else{
			return 2;
		}
	}
	return -1;
}

int Projects::new_project(const Name& n, const Path& p){
	char buffer[NAME_SIZE + 1] = {};
	n.valid(buffer);
	Path full_path = fs::absolute(p / buffer);

	if (fs::exists(full_path)){
		return 1;
	}
	else{
		if (fs::create_directory(full_path)){
			ID id{};
			uint8_t ip[4] = {127, 0, 0, 1}; // localhost
			uint8_t color = rand() % 25;

			this->projects.insert({id, Project(
				id,
				n,
				color,
				ip,
				full_path
			)});

			uint16_t path_size = strlen(full_path.c_str());
			uint16_t data_size = sizeof(ID) + sizeof(Name) + sizeof(uint8_t) + 4 * sizeof(uint8_t) + path_size;
			char raw_data[data_size + sizeof(uint16_t)] = {};
			uint16_t shift = 0;

			memcpy(raw_data + shift, &data_size, sizeof(uint16_t));
			shift += sizeof(uint16_t);

			id.get(raw_data + shift);
			shift += sizeof(ID);

			n.get(raw_data + shift);
			shift += sizeof(Name);

			memcpy(raw_data + shift, &color, sizeof(uint8_t));
			shift += sizeof(uint8_t);

			memcpy(raw_data + shift, ip, 4 * sizeof(uint8_t));
			shift += (4 * sizeof(uint8_t));

			memcpy(raw_data + shift, full_path.c_str(), path_size);

			std::ofstream stream(PROJECTS_FILE, std::ios::app | std::ios::binary | std::ios::out);

			if (stream.is_open()){
				stream.write(raw_data, data_size + sizeof(uint16_t));
				stream.close();

				return this->projects.find(id)->second.init_project();
			}
			else{
				return 3;
			}

			return 0;
		}
		else{
			return 2;
		}
	}

	return -1;
}

void Project::tell() const{
	char b_name[sizeof(Name) + 1] = {};
	char b_id[sizeof(ID) + 1] = {};
	this->name.get(b_name);
	this->id.get(b_id);
	std::cout << b_id << " -> " << b_name << std::endl;
	std::cout << "Color: " << (uint)this->color_index << std::endl;
	std::cout << "IP: " << (uint)this->ip[0] << "." << (uint)this->ip[1] << "."  << (uint)this->ip[2] << "."  << (uint)this->ip[3] << std::endl;
	std::cout << "Path: " << this->path << std::endl;
}

Project::Project(const ID& i, const Name& n, const uint8_t& ci, uint8_t* _ip, const Path& p){
	this->id = i;
	this->name = n;
	this->color_index = ci;
	memcpy(this->ip, _ip, 4 * sizeof(uint8_t));
	this->path = p;
	this->file_system = FileSystem{this->path, this->name};
}

void Projects::forget_project(const ID& id){

}

void Projects::remove_project(const ID& id){

}

void Projects::join_project_by_ip(const char* ip){

}

void Projects::tell() const{
	for (std::map<ID, Project>::const_iterator it = this->projects.begin() ; it != this->projects.end() ; it++){
		it->second.tell();
		std::cout << std::endl << "==============" << std::endl;
	}
}

bool Project::valid() const{
	if (!fs::exists(this->path)){
		return false;
	}
	return true;
}

Project& Projects::get_project(const ID& id){
	return this->projects.find(id)->second;
}

// [TotalSize][ID][Name][Color_Index][IP][Path]
Projects::Projects(){
	if (fs::exists(PROJECTS_FILE)){
		std::ifstream stream(PROJECTS_FILE, std::ios::binary | std::ios::in);

		if (stream.is_open()){
			uint16_t extraction = 0;

			while(stream.read((char*)&extraction, sizeof(uint16_t))){
				if (extraction){
					char buffer[extraction + 1] = {};
					stream.read(buffer, extraction);
					ID id{buffer};
					Project project(buffer, extraction);

					if (project.valid()){
						this->projects.insert({id, project});
					}
				}
			}

			stream.close();
		}
	}
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *v


// [ID][Name][Color_Index][IP][Path]
// [!] Add tests on the value of 'shift' to avoid out of bounds reading
Project::Project(const char* raw_data, const uint16_t& size_buffer){
	uint16_t shift = 0;

	this->id = ID{raw_data + shift};
	shift += sizeof(ID);

	this->name = Name{raw_data + shift};
	shift += sizeof(Name);

	memcpy(&this->color_index, raw_data + shift , sizeof(uint8_t));
	shift += sizeof(uint8_t);

	memcpy(this->ip, raw_data + shift, 4 * sizeof(uint8_t));
	shift += 4 * sizeof(uint8_t);

	this->path = raw_data + shift;

	this->file_system = FileSystem{this->path, this->name};
}