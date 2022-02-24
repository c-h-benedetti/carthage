#include "Folder.hpp"
#include "FileSystem.hpp"
#include <fstream>

void Folder::open(){
	this->refer_to->change_directory(std::unique_ptr<Folder>(new Folder(*this)));
	this->refer_to->open();
}

// IMPROVE: [Folder] Could the browsing be optimized? If we use a state machine in FS, we don't need this method.
FSObject* Folder::at(size_t i){
	if (i < folders.size()){
		return &(this->folders[i]);
	}
	i -= folders.size();
	if (i < files.size()){
		return &(this->files[i]);
	}
	i -= files.size();
	return &(this->versionables[i]);
}


size_t Folder::size() const{
	return this->folders.size() + this->files.size() + this->versionables.size();
}


void Folder::dispatch(const std::vector<FSObject>& segment){
	this->folders.clear();
	this->folders.reserve(this->block.nb_folders);

	this->files.clear();
	this->files.reserve(this->block.nb_files);

	this->versionables.clear();
	this->versionables.reserve(this->block.nb_versionables);


	for(size_t i = 0 ; i < segment.size() ; i++){
		if (file_raised(segment[i].data().flag)){
			this->files.push_back(File(segment[i]));
		}
		else if(folder_raised(segment[i].data().flag)){
			this->folders.push_back(Folder(segment[i]));
		}
		else if(versionable_raised(segment[i].data().flag)){
			this->versionables.push_back(Versionable(segment[i]));
		}
	}
}


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

	std::function<Versionable(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		
		b_v.content = insert + sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);

		SystemName sn{b_c.id};
		Path c_path = abs_path / sn.c_str();

		fs::create_directory(abs_path);
		fs::create_directory(c_path);

		b_c.parent = insert + sizeof(FSize);
		b_v.time = fs::last_write_time(c_path);

		Versionable ver(
			b_v,
			insert+sizeof(FSize),
			this->refer_to
		);

		Folder fdr(
			b_c, 
			insert + sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos) + sizeof(FSize), 
			this->refer_to
		);


		// 4. Updating values in objects
		this->block.nb_versionables++;
		fdr.refresh_vfs();

		return ver;
	};

	this->create_element<Versionable, 2>(
		args.name.value(),
		Extension(0),
		b_v,
		instanciating,
		this->versionables
	);
}


void Folder::new_folder(const ArgsNewFolder& args){
	FSBlock b;
	b.id.randomize();
	b.name = args.name.value();
	b.icon = args.icon.value();
	b.flag = FSType::FOLDER;
	b.parent = this->block_pos;

	std::function<Folder(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		fs::create_directory(abs_path);
		Folder fdr(
			b, 
			insert+sizeof(FSize), 
			this->refer_to
		);
		b.time = fs::last_write_time(abs_path);
		this->block.nb_folders++;

		return fdr;
	};

	this->create_element<Folder, 1>(
		args.name.value(),
		Extension(0),
		b,
		instanciating,
		this->folders
	);
}


void Folder::new_file(const ArgsNewFile& args){
	FSBlock b;
	b.id.randomize();
	b.name = args.name.value();
	b.icon = args.icon.value();
	b.extension = args.extension.value();
	b.flag = FSType::FILE;
	b.parent = this->block_pos;

	std::function<File(const Path&, const FSPos&)> instanciating = [&](const Path& abs_path, const FSPos& insert){
		std::ofstream n_fi(abs_path, std::ios::out);
		n_fi.close();

		File fi(
			b, 
			insert+sizeof(FSize), 
			this->refer_to
		);
		b.time = fs::last_write_time(abs_path);
		this->block.nb_files++;

		return fi;
	};

	this->create_element<File, 1>(
		args.name.value(),
		args.extension.value(),
		b,
		instanciating,
		this->files
	);
}


Folder::Folder(FileSystem* fs): Container(fs){}

Folder::Folder(const FSObject& obj): Container(obj){}

Folder::Folder(const FSBlock& bck, const FSPos& pos, FileSystem* fs): Container(bck, pos, fs){}
