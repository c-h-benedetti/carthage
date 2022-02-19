#ifndef FSOBJECT_HPP_INCLUDED
#define FSOBJECT_HPP_INCLUDED

#include "FSBlock.hpp"

class FileSystem;

class FSObject{

	friend class FileSystem;

protected: 

	Path path;
	bool selected = false;
	FSPos block_pos = 0;
	FSBlock block;
	FileSystem* refer_to = nullptr;

protected:

	virtual void load(){}

public:

	inline void toggle_select(){ this->selected = !this->selected; }
	inline void set_selection(const bool& b){ this->selected = b; }
	inline const FSBlock& data() const{ return this->block; }

	virtual void open(){}
	void reload_from_vfs(const bool& reset_path=true);

	FSObject() = delete;
	FSObject(FileSystem* fs);
	FSObject(const Path& p, FileSystem* fs);
	FSObject(const FSBlock& bck, const Path& p, const FSPos& pos, FileSystem* fs);

};

#endif // FSOBJECT_HPP_INCLUDED