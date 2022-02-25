#include "Folder.hpp"
#include "FileSystem.hpp"
#include <fstream>

// IMPROVE: [Folder] Implement the join() method to build the path in the FileSystem.
// IMPROVE: [Folder] Could the browsing be optimized? If we use a state machine in FS, we don't need this method.

// DEBUG: [Folder] In dispatch, check that the values nb_x match the number of elements effectively seen in the loop.

/**
 * Filters the content received in segment.
 */
void Folder::dispatch(const std::vector<FSObject>& segment){
	this->content.clear();
	this->content.reserve(this->block.nb_folders + this->block.nb_files + this->block.nb_versionables);

	for(const FSObject& obj : segment){
		if (!removed_raised(obj.data().flag)){
			this->content.push_back(obj);
		}
	}
}


/**
 * Creating a new file
 */
void Folder::new_file(const ArgsNewFile& args){
	FSBlock b;
	b.id.randomize();
	b.name = args.name.value();
	b.icon = args.icon.value();
	b.extension = args.extension.value();
	b.flag = FSType::FILE;
	b.parent = this->block_pos;

	std::function<FSObject(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		std::ofstream n_fi(abs_path, std::ios::out);
		n_fi.close();

		FSObject fi(
			this->refer_to,
			b, 
			insert+sizeof(FSize)
		);
		b.time = fs::last_write_time(abs_path);
		this->block.nb_files++;

		return fi;
	};

	this->create_element<1>(
		b,
		instanciating,
		this->content
	);
}


/**
 * Creating a new folder
 */
void Folder::new_folder(const ArgsNewFolder& args){
	FSBlock b;
	b.id.randomize();
	b.name = args.name.value();
	b.icon = args.icon.value();
	b.flag = FSType::FOLDER;
	b.parent = this->block_pos;

	std::function<FSObject(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		fs::create_directory(abs_path);
		FSObject fdr(
			this->refer_to,
			b, 
			insert+sizeof(FSize) 
		);
		b.time = fs::last_write_time(abs_path);
		this->block.nb_folders++;

		return fdr;
	};

	this->create_element<1>(
		b,
		instanciating,
		this->content
	);
}


/**
 * Creating a new versionable
 */
void Folder::new_versionable(const ArgsNewVersionable& args){

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

	std::function<FSObject(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		
		b_v.content = insert + sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);

		SystemName sn{b_c.id};
		Path c_path = abs_path / sn.c_str();

		fs::create_directory(abs_path);
		fs::create_directory(c_path);

		b_c.parent = insert + sizeof(FSize);
		b_v.time = fs::last_write_time(c_path);

		FSObject ver(
			this->refer_to,
			b_v,
			insert+sizeof(FSize)
		);

		FSObject fdr(
			this->refer_to,
			b_c, 
			insert + sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos) + sizeof(FSize)
		);


		// 4. Updating values in objects
		this->block.nb_versionables++;
		fdr.override_vfs();

		return ver;
	};

	this->create_element<2>(
		b_v,
		instanciating,
		this->content
	);
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #       CONSTRUCTORS                                                                      #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


Folder::Folder(FileSystem& fs): Container(fs){}

Folder::Folder(const FSObject& obj): Container(obj){}

Folder::Folder(FileSystem& fs, const FSBlock& bck, const FSPos& pos): Container(fs, bck, pos){}
