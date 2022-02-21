#ifndef FILE_HPP_INCLUDED
#define FILE_HPP_INCLUDED

#include "FileSysObject.hpp"

#define UNIX_BIN 0x464c457f

class File : public FileSystemObject{

public:

	enum class LaunchStatus{
		UNKNOWN,
		EXIST,
		LAUNCHED,
		ENDED
	};

	enum class Type{
		UNKNOWN,
		BLEND,
		TXT,
		ODT,
		HTML,
		UNIX_BINARY,
		CPP,
		HPP,
		C,
		H,
		ZIP
	};

private:

	Type type = Type::UNKNOWN;
	void determine_type(const fs::directory_entry& d);

public:

	File();
	File(const std::string& s);
	File(const std::string& s, const fs::directory_entry& d);

	int launch();
	Type get_type() const;
};

#endif //FILE_HPP_INCLUDED