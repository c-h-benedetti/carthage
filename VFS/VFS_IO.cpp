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

constexpr bool is_big = std::endian::native == std::endian::big;

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
		this->raise(VFSError::OPEN_FAIL);
		this->stream = nullptr;
		return this->state;
	}
}


void VFSReader::close(){
	if (this->stream){
		this->stream->close();
	}
	this->stream = nullptr;
}


void VFSReader::init(){
	if (is_big){
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
	}
}


VFSReader& VFSReader::sequence(const FSPos& p){
	if (!this->state){
		this->stream->seekg(p);
	}
	return *this;
}


/**
 * This function uses the head's position which is already set for this stream object. It is not reset to 0.
 */
int VFSReader::linear_browsing(std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f){
	if (this->state){
		return this->state;
	}
	else{

		FSize seg_size = 0;
		FSPos next = 0;
		FSPos current = 0;
		InputBuffer buffer;

		while (this->stream->peek() != EOF){
			
			this->stream->read((char*)&seg_size, sizeof(FSize));
			AfterReading<FSize>::behavior(seg_size);
			
			current = this->stream->tellg();
			buffer.reset();
			buffer.read_from(*(this->stream), seg_size * sizeof(FSBlock) + sizeof(FSPos));
			FSBlock blocks[seg_size];

			for (size_t i = 0 ; i < seg_size ; i++){
				buffer.get<FSBlock>(blocks[i]);
			}

			buffer.get<FSPos>(next);

			f(seg_size, blocks, current, next);
		}

		return 0;
	}
}

// IMPROVE: [FSBlock] A FSBlock should contain a constant variable making it easy to know if what we read from the VFS is corrupted. Ex: Add 8 bytes of zeros in the middle and check that we have a zero at reading.

/**
 * #cont_pos is a segment address.
 */
int VFSReader::browse_block_content(const FSPos& cont_pos, std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f){
		
	if (this->state){
		return this->state;
	}
	else{

		InputBuffer buffer;
		FSPos c_pos = cont_pos;
		FSPos next = 0;
		FSize size = 0;

		while (c_pos){
			this->stream->seekg(c_pos);
			
			this->stream->read((char*)&size, sizeof(FSize));
			AfterReading<FSize>::behavior(size);

			FSBlock blocks[size];

			buffer.read_from(*(this->stream), size * sizeof(FSBlock) + sizeof(FSPos));
			c_pos += sizeof(FSize);

			for (size_t i = 0 ; i < size ; i++){
				buffer.get<FSBlock>(blocks[i]);
			}

			buffer.get<FSPos>(next);

			f(size, blocks, c_pos, next);

			c_pos = next;
		}

		return 0;
	}
}


int VFSReader::probe_fsblock(const FSPos& pos, FSBlock& block){
	
	if (this->state){
		return this->state;
	}
	else{
		this->stream->seekg(pos);
		this->stream->read((char*)&block, sizeof(FSBlock));
		AfterReading<FSBlock>::behavior(block);
		return 0;
	}
}


int VFSReader::backtrack(std::vector<FSBlock>& blocks, const FSPos& pos){
	if (this->state){
		return this->state;
	}
	else{

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

		return 0;
	}
}

/*
void VFSReader::recursive_hierarchy(
	size_t depth, 
	const FSBlock& block, 
	const Path& p, 
	std::function<void(const FSBlock&, const size_t&, const Path&)>& f,
	std::function<void(const FSBlock&, const size_t&, const Path&, const std::vector<FSBlock>& content)>& f2
){
	f(block, depth, p);
	if (block.content){
		std::vector<FSBlock> blocks;
		FSPos c_pos = block.content;
		FSPos start_pos = c_pos;
		FSize size = 0;
		InputBuffer buffer;

		while(c_pos){
			this->stream->seekg(c_pos);
			this->read<FSize>(size);
			FSBlock read_blocks[size];
			blocks.reserve(blocks.size() + size);
			buffer.reset();
			buffer.read_from(*(this->stream), sizeof(FSBlock) * size + sizeof(FSPos));

			for (size_t i = 0 ; i < size ; i++){
				blocks.push_back(FSBlock());
				buffer.get<FSBlock>(blocks.back());
			}

			buffer.get<FSPos>(c_pos);
		}

		f2(block, depth, p, blocks);

		for(const FSBlock& blc : blocks){
			SystemName sn{block.id};
			this->recursive_hierarchy(depth+1, blc, p / sn.c_str(), f, f2);
		}
	}
}


int VFSReader::browse_hierarchy(
	const FSPos& root_pos, 
	std::function<void(const FSBlock&, const size_t&, const Path&)> f,
	std::function<void(const FSBlock&, const size_t&, const Path&, const std::vector<FSBlock>& content)> f2
){
	if (this->state){
		return this->state;
	}
	else{
		
		this->stream->seekg(root_pos);
		FSBlock block;
		this->read<FSBlock>(block);

		SystemName sn{block.id};
		this->recursive_hierarchy(0, block, sn.c_str(), f, f2);

		return 0;
	}
}
*/

FSize VFSReader::fetch_level(const FSBlock& b, std::deque<FSBlock>& blocks){
	FSize total = 0;
	if (b.content > 0){
		this->browse_block_content(b.content, [&](const FSize& seg_size, const FSBlock* blcs, const FSPos& stt, const FSPos& next){
			for (size_t i = 0 ; i < seg_size ; i++){
				if (!removed_raised(blcs[i].flag)){
					blocks.push_back(blcs[i]);
					total++;
					std::cout << b.name << "/" << blcs[i].name << std::endl;
				}
			}
		});
	}

	return total;
}


void VFSReader::iterative_bfs_hierarchy(const FSPos& start){
	FSBlock current;
	this->sequence(start).read<FSBlock>(current);
	std::deque<FSBlock> file;
	std::deque<FSize> len_file;
	file.push_back(current);
	len_file.push_back(1);
	FSize count = 0;

	while (file.size() > 0){
		current = file.front();
		file.pop_front();
		// Create the content of current here.
		
		// ---
		len_file.push_back(fetch_level(current, file));
		count++;

		if (count >= len_file.front()){ // Changing of depth
			std::cout << "===" << std::endl;
			count = 0;
			len_file.pop_front();
		}
	}
}


VFSReader::VFSReader(const Path& p, std::function<void()> c): vfs_path(p), callback(c){
	this->init();
	if (!fs::exists(this->vfs_path) || !fs::is_regular_file(this->vfs_path)){
		throw "The path provided for the VFS is not valid.";
	}
	this->open();
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
		this->stream = nullptr;
		this->raise(VFSError::OPEN_FAIL);
		return this->state;
	}
}
	

void VFSWriter::close(){
	if (this->stream){
		this->stream->close();
	}
	this->stream = nullptr;
}


void VFSWriter::init(){
	if (is_big){
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
	}
}


int VFSWriter::blank_segments(const FSize& count, FSPos& insert){
	if (this->state || !count){
		this->raise(VFSError::INVALID_COUNT);
		return this->state;
	}
	else{
		constexpr size_t b_size = sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);
		BasicBuffer<b_size> segment;
		
		FSize s = 1;
		BeforeWriting<FSize>::behavior(s);
		segment.add<FSize>(s);

		segment.add<FSBlock>(FSBlock());
		segment.add<FSPos>(0);
		
		OutputBuffer buffer;
		for (size_t i = 0 ; i < count ; i++){
			buffer.copy_from(&segment, b_size);
		}

		this->stream->seekp(0, std::ios_base::end);
		insert = this->stream->tellp();
		buffer.write_to(*(this->stream));

		return 0;
	}
}


int VFSWriter::blank_segment(const FSize& count, FSPos& insert){

	if (this->state || !count){
		this->raise(VFSError::INVALID_COUNT);
		return this->state;
	}
	else{
		OutputBuffer buffer;

		buffer.add<FSize>(count);

		FSBlock block{};
		for (size_t i = 0 ; i < count ; i++){
			buffer.add<FSBlock>(block);
		}

		buffer.add<FSPos>(0);

		this->stream->seekp(0, std::ios_base::end);
		insert = this->stream->tellp();
		buffer.write_to(*(this->stream));

		return 0;
	}
}


VFSWriter& VFSWriter::sequence(const FSPos& pos){
	if (!this->state){
		this->stream->seekp(pos);
	}
	return *this;
}


VFSWriter::VFSWriter(const Path& p, std::function<void()> c, const bool& reset): vfs_path(p), callback(c){
	this->init();
	if (!fs::exists(p) || reset){
		touch(p);
	}
	this->open();
}


VFSWriter::~VFSWriter(){
	this->close();
	this->callback();
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
VFSWriter VFS_IO::ask_writer(const bool& reset){
	this->writers_count++;
	
	return VFSWriter(this->vfs_path, [&](){
		this->writers_count--;
	}, reset);
}


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

