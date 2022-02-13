#include "FSBlock.hpp"
#include <iostream>

FSBlock::FSBlock(){}

auto print_last_write_time = [](std::filesystem::file_time_type const& ftime) {
    std::time_t cftime = std::chrono::system_clock::to_time_t(
        std::chrono::file_clock::to_sys(ftime));
    std::cout << "Object was written on: " << std::asctime(std::localtime(&cftime));
};

void FSBlock::tell() const{
	char n_buf[sizeof(Name) + 1] = {};
	char id_buf[sizeof(ID) + 1] = {};
	this->name.get(n_buf);
	this->id.get(id_buf);

	std::cout << this->content_address << std::endl;
	std::cout << id_buf << std::endl;
	std::cout << n_buf << std::endl;
	std::cout << static_cast<int>(this->type) << std::endl;
	std::cout << this->nb_folders << std::endl;
	std::cout << this->nb_files << std::endl;
	std::cout << this->nb_versionables << std::endl;
	std::cout << this->__blank__ << std::endl;
	print_last_write_time(this->last_edition_date);
	std::cout << this->metadata_address << std::endl;
	std::cout << this->parent_address << std::endl;
	std::cout << this->last_editor << std::endl;
	std::cout << this->permissions << std::endl;
	std::cout << this->ftype << std::endl;
	std::cout << this->dependencies_address << std::endl;
	std::cout << this->owner << std::endl;
	std::cout << this->group << std::endl;
}