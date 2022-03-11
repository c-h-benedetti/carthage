#include "VFS_IO.hpp"
#include <bit>

/**
 * The VFS will store informations in little-endian, it is the most used endianness.
 * If the system uses the same endianness as the VFS, no operation has to be performed.
 */

constexpr bool is_not_mixed = ((std::endian::native == std::endian::big) || (std::endian::native == std::endian::little));
static_assert(
	is_not_mixed,
	"Mixed endianness is not handled"
);



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       VFS READER IMPLEMENTATION                                                             #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

int VFSReader::open(){
	this->stream = std::unique_ptr<std::ifstream>(new std::ifstream(this->vfs_path, std::ios::in | std::ios::binary));
	if (this->stream->is_open()){
		return 0;
	}
	else{
		return 1;
	}
}


int VFSReader::close(){
	if (this->stream){
		this->stream->close();
	}
	this->stream = nullptr;
	return 0;
}

constexpr bool is_big = std::endian::native == std::endian::big;

void VFSReader::init(){
	/*if (is_big){
		AfterReading<FSize>::behavior = [](FSize& i){
			mirror_bytes<FSize>(i);
		};


		AfterReading<FSPos>::behavior = [](FSPos& i){
			mirror_bytes<FSPos>(i);
		};


		AfterReading<FSBlock>::behavior = [](FSBlock& b){
			mirror_bytes<FSPos>(b.content);
			mirror_bytes<FSize>(b.nb_files);
			mirror_bytes<FSize>(b.nb_folders);
			mirror_bytes<FSize>(b.nb_versionables);
			mirror_bytes<uint16_t>(b.icon);
			mirror_bytes<FSTime>(b.time);
			mirror_bytes<FSPos>(b.metadata);
			mirror_bytes<FSPos>(b.parent);
			mirror_bytes<UID>(b.editor);
			mirror_bytes<FSPos>(b.dependencies);
			mirror_bytes<UID>(b.owner);
			mirror_bytes<UID>(b.group);
		};
	}*/
}


VFSReader& VFSReader::sequence(const FSPos& p){
	this->state = this->open();
	if (!this->state){
		this->stream->seekg(p);
	}
	return *this;
}


VFSReader& VFSReader::read_fsize(FSize& s){
	if (!this->state){
		this->stream->read((char*)&s, sizeof(FSize));
		AfterReading<FSize>::behavior(s);
	}
	return *this;
}


VFSReader& VFSReader::read_fsblock(FSBlock& b){
	if (!this->state){
		this->stream->read((char*)&b, sizeof(FSBlock));
		AfterReading<FSBlock>::behavior(b);
	}
	return *this;
}


VFSReader& VFSReader::read_fspos(FSPos& p){
	if (!this->state){
		this->stream->read((char*)&p, sizeof(FSPos));
		AfterReading<FSPos>::behavior(p);
	}
	return *this;
}


int VFSReader::linear_browsing(std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f){
	if (this->open()){
		return 1;
	}
	else{

		FSize seg_size = 0;
		FSPos next = 0;
		FSPos current = 0;

		while (this->stream->peek() != EOF){
			
			this->stream->read((char*)&seg_size, sizeof(FSize));
			AfterReading<FSize>::behavior(seg_size);
			
			current = this->stream->tellg();
			this->buffer.reset();
			this->buffer.read_from(this->get_stream(), seg_size * sizeof(FSBlock) + sizeof(FSPos));
			FSBlock blocks[seg_size];

			for (size_t i = 0 ; i < seg_size ; i++){
				this->buffer.get<FSBlock>(blocks[i]);
			}

			this->buffer.get<FSPos>(next);

			f(seg_size, blocks, current, next);
		}

		this->close();
		return 0;
	}
}

// IMPROVE: [VFSReader] We can open the object in the constructor eand close it in the destructor instead of summoning both these functions manually.

// IMPROVE: [VFSReader][VFSWriter] Can be optimized, the dynamic buffer is certainly not useful in all operations.

/**
 * Returns 0 on success.
 * If the return code is in the range of VFSReader::open() error codes, opening the VFS failed.
 * Returns 10 if the content's position is 0 (nullptr)
 */
int VFSReader::browse_block_content(const FSPos& cont_pos, std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f){

	if (cont_pos){
		int status = this->open();
		
		if (status){
			this->close();
			return status;
		}
		else{
			FSPos c_pos = cont_pos;
			FSPos next = 0;
			FSize size = 0;

			while (c_pos){
				this->stream->seekg(c_pos);
				this->buffer.read_from(this->get_stream(), sizeof(FSize));
				this->buffer.get<FSize>(size);
				FSBlock blocks[size];
				this->buffer.read_from(this->get_stream(), size * sizeof(FSBlock) + sizeof(FSPos));
				c_pos += sizeof(FSize);

				for (size_t i = 0 ; i < size ; i++){
					this->buffer.get<FSBlock>(blocks[i]);
				}

				this->buffer.get<FSPos>(next);

				f(size, blocks, c_pos, next);

				c_pos = next;
			}

			this->close();

			return 0;
		}

		return 0;
	}
	else{
		return 10;
	}
}


int VFSReader::probe_fsblock(const FSPos& pos, FSBlock& block){
	if (pos){
		int status = this->open();

		if (status){
			this->close();
			return status;
		}
		else{
			this->stream->seekg(pos);
			this->buffer.read_from(this->get_stream(), sizeof(FSBlock));
			this->buffer.get<FSBlock>(block);
			this->close();
			return 0;
		}

		return 0;
	}
	else{
		return 10;
	}
}


int VFSReader::backtrack(std::vector<FSBlock>& blocks, const FSPos& pos){
	if (pos){
		if (this->open()){
			return 2;
		}
		FSPos p_pos = pos;
		blocks.clear();
		FSBlock block;

		while (p_pos){
			this->stream->seekg(p_pos);
			this->stream->read((char*)&block, sizeof(FSBlock));
			AfterReading<FSBlock>::behavior(block);
			blocks.push_back(block);
			p_pos = block.parent;
		}

		this->close();
		return 0;
	}
	else{
		return 1;
	}
}


VFSReader::VFSReader(const Path& p, std::function<void()> c): vfs_path(p), callback(c){
	this->init();
	if (!fs::exists(this->vfs_path) || !fs::is_regular_file(this->vfs_path)){
		throw "The path provided for the VFS is not valid.";
	}
}


VFSReader::~VFSReader(){
	this->close();
	this->callback();
}



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       VFS WRITER IMPLEMENTATION                                                             #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

int VFSWriter::open(){
	this->stream = std::unique_ptr<std::ofstream>(new std::ofstream(this->vfs_path, std::ios::in | std::ios::out | std::ios::binary));
	if (this->stream->is_open()){
		return 0;
	}
	else{
		return 1;
	}
}
	

int VFSWriter::close(){
	if (this->stream){
		this->stream->close();
	}
	this->stream = nullptr;
	return 0;
}


void VFSWriter::init(){
	/*if (is_big){
		BeforeWriting<FSize>::behavior = [](FSize& i){
			mirror_bytes<FSize>(i);
		};


		BeforeWriting<FSPos>::behavior = [](FSPos& i){
			mirror_bytes<FSPos>(i);
		};


		BeforeWriting<FSBlock>::behavior = [](FSBlock& b){
			mirror_bytes<FSPos>(b.content);
			mirror_bytes<FSize>(b.nb_files);
			mirror_bytes<FSize>(b.nb_folders);
			mirror_bytes<FSize>(b.nb_versionables);
			mirror_bytes<uint16_t>(b.icon);
			mirror_bytes<FSTime>(b.time);
			mirror_bytes<FSPos>(b.metadata);
			mirror_bytes<FSPos>(b.parent);
			mirror_bytes<UID>(b.editor);
			mirror_bytes<FSPos>(b.dependencies);
			mirror_bytes<UID>(b.owner);
			mirror_bytes<UID>(b.group);
		};
	}*/
}


int VFSWriter::override_fsblock(const FSPos& pos, const FSBlock& block){
	if (pos && !this->open()){
		FSBlock data = block;
		BeforeWriting<FSBlock>::behavior(data);
		this->stream->write((char*)&data, sizeof(FSBlock));
		this->close();
		return 0;
	}
	else{
		return 1;
	}
}
	
int VFSWriter::override_address(const FSPos& pos, const FSPos& next_val){
	if (pos && !this->open()){
		FSPos data = next_val;
		BeforeWriting<FSPos>::behavior(data);
		this->stream->write((char*)&data, sizeof(FSPos));
		this->close();
		return 0;
	}
	else{
		return 1;
	}
}

int VFSWriter::override_fsize(const FSPos& pos, const FSize& size){
	if (pos && !this->open()){
		FSize data = size;
		BeforeWriting<FSize>::behavior(data);
		this->stream->write((char*)&data, sizeof(FSize));
		this->close();
		return 0;
	}
	else{
		return 1;
	}
}

int VFSWriter::blank_segments(const FSize& count, FSPos& insert){
	const size_t b_size = sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);
	BasicBuffer<b_size> buffer;
	FSize s = 1;
	buffer.add<FSize>(s);
	this->buffer.reset();

	for (size_t i = 0 ; i < count ; i++){
		this->buffer.copy_from(&buffer, b_size);
	}

	if (this->open()){
		return 1;
	}
	else{
		this->stream->seekp(0, std::ios_base::end);
		insert = this->stream->tellp();
		this->buffer.write_to(this->get_stream());
		this->close();
		return 0;		
	}
}

int VFSWriter::blank_segment(const FSize& count, FSPos& insert){
	this->buffer.reset();
	FSBlock block{};
	
	this->buffer.add<FSize>(count);
	for (size_t i = 0 ; i < count ; i++){
		this->buffer.add<FSBlock>(block);
	}
	this->buffer.add<FSPos>(0);

	if (this->open()){
		return 1;
	}
	else{
		this->stream->seekp(0, std::ios_base::end);
		insert = this->stream->tellp();
		this->buffer.write_to(this->get_stream());
		this->close();
		return 0;		
	}
}


VFSWriter& VFSWriter::sequence(const FSPos& pos){
	if (this->open()){
		this->state = 1;
	}
	else{
		this->stream->seekp(pos);
	}
	return *this;
}


VFSWriter& VFSWriter::add_fsize(const FSize& size){
	if (!this->state){
		FSize s = size;
		BeforeWriting<FSize>::behavior(s);
		this->stream->write((char*)&s, sizeof(FSize));
	}
	return *this;
}


VFSWriter& VFSWriter::add_fsblock(const FSBlock& blc){
	if (!this->state){
		FSBlock b = blc;
		BeforeWriting<FSBlock>::behavior(b);
		this->stream->write((char*)&b, sizeof(FSBlock));
	}
	return *this;
}


VFSWriter& VFSWriter::add_fspos(const FSPos& pos){
	if (!this->state){
		FSPos p = pos;
		BeforeWriting<FSPos>::behavior(p);
		this->stream->write((char*)&p, sizeof(FSPos));
	}
	return *this;
}


VFSWriter::VFSWriter(const Path& p, std::function<void()> c): vfs_path(p), callback(c){
	this->init();
	if (!fs::exists(p)){
		touch(p);
	}
}


VFSWriter::~VFSWriter(){
	this->close();
}


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       VFS IO IMPLEMENTATION                                                                 #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


// Reader doit pouvoir renvoyer autant d'instances que possible si aucun writer n'est actif
VFSReader VFS_IO::ask_reader(){

	this->readers_count++;

	return VFSReader(this->vfs_path, [&](){
		this->readers_count--;
	});
}


// Writer doit s'assurer qu'aucun reader n'est en cours et ne peut instancier qu'un writer à la fois.
// Il faudrait pouvoir implémenter une queue qui rend les writers prioritaires sur les readers.
// Les readers sont ajoutés en fin de queue dans l'ordre, les writers sont ajoutés en tête de queue.
// La dequeue et ces fonctions vont devoir utiliser des mutex, on des condition variables.
// Pour ne pas avoir à connaitre tous les types de tous les appelants, la queue sera une queue de lambda expression avec capture de contexte.
// La fonction sera executée quand elle sera executée. L'appelant doit quand même rester bloqué jusqu'à ce que la requête soit terminée (pour la lecture en tous cas)
VFSWriter VFS_IO::ask_writer(){
	this->writers_count++;
	
	return VFSWriter(this->vfs_path, [&](){
		this->writers_count--;
	});
}

// IMPROVE: [VFS_IO] Create the empty elements here if they don't exist.
VFS_IO::VFS_IO(const Path& u_root): user_root(u_root), vfs_path(u_root / CARTHAGE_DIR / VFS_DESCRIPTOR){

	// If the user-provided path doesn't exist or isn't a directory.
	if (!fs::exists(u_root) || !fs::is_directory(u_root)){
		BasicBuffer<128> msg(0);
		msg.copy_from("The path \"").copy_from(u_root.c_str()).copy_from("\" doesn't exist, or is not a directory.");
		throw std::invalid_argument(msg.c_str());
	}

	Path ctg = u_root / CARTHAGE_DIR;
	if (!fs::exists(ctg)){
		if (!fs::create_directory(ctg)){
			BasicBuffer<128> msg(0);
			msg.copy_from("Impossible to create the Carthage hidden directory at the specified path: ").copy_from(u_root.c_str());
			throw std::invalid_argument(msg.c_str());
		}
	}
	else{
		if (!fs::is_directory(ctg)){
			BasicBuffer<128> msg(0);
			msg.copy_from("A file named \".carthage\" already exists at the specified path: ").copy_from(u_root.c_str()).copy_from(" impossible to create the folder");
			throw std::invalid_argument(msg.c_str());
		}
	}

	ctg /= VFS_DESCRIPTOR;
	if (fs::exists(ctg) && !fs::is_regular_file(ctg)){
		BasicBuffer<128> msg(0);
			msg.copy_from("An object at the specified path: ").copy_from(u_root.c_str()).copy_from(" makes impossible to create the VFS.");
			throw std::invalid_argument(msg.c_str());
	}
}


Path VFS_IO::make_path(const Path& base_path, FSBlock& block){
	SystemName sn{block.id, block.extension};
	return base_path / sn.c_str();
}

// Si un writer est demandé, on ne doit plus accepter les readers (on les met en attente)
