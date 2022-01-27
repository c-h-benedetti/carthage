#ifndef PROJECT_CLASS_HEADER_HPP_INCLUDED
#define PROJECT_CLASS_HEADER_HPP_INCLUDED

#include <map>
#include "utils.h"
#include "FileSystem.hpp"
#include "Folder.hpp"

class Project{
	ID id;
	Name name;
	uint8_t color_index;
	uint8_t ip[4];
	Path path;

	FileSystem file_system;

public:

	void tell() const;
	bool valid() const;
	int init_project() const;

	inline FileSystem& filesystem(){return this->file_system;}

	Project(const ID& i, const Name& n, const uint8_t& ci, uint8_t* _ip, const Path& p);
	Project(const char* raw_data, const uint16_t& size_buffer);
};


class Projects{

	std::map<ID, Project> projects;

public:

	int new_project(const Name& n, const Path& p);
	void forget_project(const ID& id);
	void remove_project(const ID& id);
	void join_project_by_ip(const char* ip);
	Project& get_project(const ID& id);
	void tell() const;

	Projects();
};


#endif // PROJECT_CLASS_HEADER_HPP_INCLUDED