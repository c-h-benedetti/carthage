#include "Folder.hpp"
#include "FileSystem.hpp"
#include <fstream>

// DEBUG: [Folder] In dispatch, check that the values nb_x match the number of elements effectively seen in the loop.

/**
 * Filters the content received in segment by skiping removed items.
 */
void Folder::dispatch(const std::vector<FSObject>& segment){
	this->content.clear();
	this->content.reserve(this->block.nb_folders + this->block.nb_files + this->block.nb_versionables);

	// IMPROVE: [Folder] Check that nothing with a CURRENT or a VERSION flag is here.
	for(const FSObject& obj : segment){
		if (!removed_raised(obj.get_data().flag)){
			this->content.push_back(obj);
		}
	}
}


int Folder::open(){
	return this->load();
}


bool Folder::accepts_versionables() const{
	bool found = false;
	Container* prev = this->previous;

	while (prev && !found){
		found = versionable_raised(prev->get_data().flag);
		prev = (Container*)prev->get_previous();
	}

	return !found;
}


/**
 * Creating a new file
 */
int Folder::create_file(const ArgsNewFile& args){
	FSBlock b;
	b.id.randomize();
	b.name = args.name.value();
	b.icon = args.icon.value();
	b.extension = args.extension.value();
	b.flag = FSType::FILE;
	b.parent = this->block_pos;

	std::function<void(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		if (touch(abs_path)){
			return;
		}

		b.time = fs::last_write_time(abs_path);

		FSObject fi(
			this->refer_to,
			b, 
			insert + sizeof(FSize)
		);
		fi.chain(this);

		if (fi.override_vfs()){
			return;
		}

		this->block.nb_files++;
		this->content.push_back(fi);
	};

	return this->create_element(
		1,
		b,
		instanciating
	);
}


/**
 * Creating a new folder
 */
int Folder::create_folder(const ArgsNewFolder& args){
	FSBlock b;
	b.id.randomize();
	b.name = args.name.value();
	b.icon = args.icon.value();
	b.flag = FSType::FOLDER;
	b.parent = this->block_pos;

	std::function<void(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		if (!fs::create_directory(abs_path)){
			return;
		}

		b.time = fs::last_write_time(abs_path);

		FSObject fdr(
			this->refer_to,
			b, 
			insert+sizeof(FSize) 
		);
		fdr.chain(this);

		if (fdr.override_vfs()){
			return;
		}

		this->block.nb_folders++;
		this->content.push_back(fdr);
	};

	// IMPROVE: [Folder] We must handle the error code returned by this function.
	return this->create_element(
		1,
		b,
		instanciating
	);
}

// IMPROVE: [Folder] Evaluate the functions accepts_xxx() before doing anything in create_xxx()

/**
 * Creating a new versionable
 */
int Folder::create_versionable(const ArgsNewVersionable& args){

	FSBlock b_v;
	b_v.id.randomize();
	b_v.name = args.name.value();
	b_v.nb_folders = 1;
	b_v.icon = args.icon.value();
	b_v.flag = FSType::VERSIONABLE;
	b_v.parent = this->block_pos;

	FSBlock b_c;
	b_c.id.randomize();
	b_c.icon = 0;
	b_c.flag = pair_flags(FSType::CURRENT, FSType::FOLDER);

	std::function<void(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		
		SystemName sn{b_c.id};
		Path c_path = abs_path / sn.c_str();

		if (!fs::create_directory(abs_path) || !fs::create_directory(c_path)){
			return;
		}

		b_v.time = fs::last_write_time(abs_path);
		b_c.time = fs::last_write_time(c_path);
		b_v.content = insert + sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);
		b_c.parent = insert + sizeof(FSize);
		

		FSObject ver(
			this->refer_to,
			b_v,
			insert+sizeof(FSize)
		);
		ver.chain(this);

		// This object is just to be written in the VFS, it must not be added in the content on this Container.
		// It must not be chained either.
		FSObject fdr(
			this->refer_to,
			b_c, 
			insert + sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos) + sizeof(FSize)
		);


		if (ver.override_vfs() || fdr.override_vfs()){
			return;
		}
		
		// 4. Updating values in objects
		this->block.nb_versionables++;
		this->content.push_back(ver);
	};

	return this->create_element(
		2,
		b_v,
		instanciating
	);
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Folder::Folder(FileSystem& fs): Container(fs){}

Folder::Folder(const FSObject& obj): Container(obj){}

Folder::Folder(FileSystem& fs, const FSBlock& bck, const FSPos& pos): Container(fs, bck, pos){}
