#ifndef VFS_ARGUMENTS_BASE_HPP_INCLUDED
#define VFS_ARGUMENTS_BASE_HPP_INCLUDED

#include <functional>
#include "utils.h"

#define SIZE_OF_DESCRIPTION 128
#define SIZE_OF_CATEGORY 32

// IMPROVE: [Description] Check that the Description class will always finish by the NULL character
/**
 * Description of the argument as it will be displayed to the user of all client apps.
 */
class Description : public StackSegment<SIZE_OF_DESCRIPTION>{
public:

	Description(const char* c): StackSegment<SIZE_OF_DESCRIPTION>(c, strlen(c)){}
};

// IMPROVE: [Category] Check that the Category class will always finish by the NULL character
/**
 * Textual representation of the type of this class to be identified by a client app.
 */
class Category : public StackSegment<SIZE_OF_CATEGORY>{
public:

	Category(const char* c): StackSegment<SIZE_OF_CATEGORY>(c, strlen(c)){}
};

// IMPROVE: [BaseArg<T>] Find the best way to implement the usage of the validity() function.
// IMPROVE: [BaseArg<T>] Maybe that the validity() function should have a way to take parameters.
/**
 * Skeleton to be specialized to create an argument for a VFS method.
 */
template <typename T>
class BaseArg{

protected:

	bool filled = false;
	T data;
	const std::function<bool(const T&)> validity;

public:

	const Name displayed_name;
	const Description description;
	const bool mandatory = false;
	const Category category;

public:

	BaseArg(const Category& cat, const Name& n, const bool& m, const char* d): category(cat), displayed_name(n), mandatory(m), description(d){}

	BaseArg(const Category& cat, const Name& n, const bool& m, const char* d, const T& t, std::function<bool(const T&)> va): category(cat), displayed_name(n), mandatory(m), description(d), validity(va){
		this->set(t);
	}

	inline void set(const T& t){
		this->data = t;
		this->filled = true;
	}

	BaseArg<T>& operator=(const T& t){
		this->set(t);
		return *this;
	}

	inline bool is_set_and_mandatory() const{ return (this->filled && this->mandatory); }
	inline bool is_set() const{ return this->filled; }
	inline const T& value() const{ return this->data; }
};


class ArgFileName : public BaseArg<Name>{

public: 

	ArgFileName& operator=(const char* c){
		this->set(c);
		return *this;
	}

	ArgFileName(): BaseArg<Name>(
		"NAME", 
		"File Name", 
		true, 
		"Name of the new file.", 
		"New File", 
		[&](const Name& n){return true;}
	){}
};


class ArgFileExtension : public BaseArg<Extension>{

public:

	ArgFileExtension& operator=(const char* c){
		this->set(c);
		return *this;
	}

	ArgFileExtension(): BaseArg<Extension>(
		"EXT", 
		"File Type", 
		true, 
		"Type of the new file (extension)."
	){}
};


class ArgFolderName : public BaseArg<Name>{

public:

	ArgFolderName& operator=(const char* c){
		this->set(c);
		return *this;
	}

	ArgFolderName(): BaseArg<Name>(
		"NAME", 
		"Folder Name", 
		true, 
		"Name of the new folder.", 
		"New Folder", 
		[&](const Name& n){return true;} // Check for validity and availability, must know other names from the folder
	){}
};


class ArgIcon : public BaseArg<uint16_t>{

public:

	ArgIcon& operator=(const uint16_t& c){
		this->set(c);
		return *this;
	}

	ArgIcon(): BaseArg<uint16_t>(
		"ICON", 
		"Icon", 
		false, 
		"Icon for this new item.", 
		1, 
		[&](const uint16_t& n){return true;} // Check for validity and availability, must know other names from the folder
	){}
};

#include "arguments.tpp"

#endif // VFS_ARGUMENTS_BASE_HPP_INCLUDED