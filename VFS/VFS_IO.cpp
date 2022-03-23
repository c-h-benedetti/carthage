#include "VFS_IO.hpp"
#include "FSType.hpp"
#include "OutputBuffer.hpp"
#include <bit>
#include <utility>

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
	if (this->stream){
		if (this->stream->is_open()){
			return 0;
		}
		else{
			this->close();
		}
	}

	this->stream = std::unique_ptr<std::ifstream>(new std::ifstream(this->vfs_io.vfs_path, std::ios::in | std::ios::binary));

	if (this->stream->is_open()){
		return 0;
	}
	else{
		this->raise(1);
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
	if (!this->state && !this->open()){	
		this->stream->seekg(p);
	}
	else{
		this->raise(1);
	}
	return *this;
}


/**
 * This function uses the head's position which is already set for this stream object. It is not reset to 0.
 */
VFSReader& VFSReader::linear_browsing(std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f){
	if (!this->state && !this->open()){
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
	}
	else{
		this->raise(2);
	}

	return *this;
}

// IMPROVE: [FSBlock] A FSBlock should contain a constant variable making it easy to know if what we read from the VFS is corrupted. Ex: Add 8 bytes of zeros in the middle and check that we have a zero at reading.

/**
 * #cont_pos is a segment address.
 */
VFSReader& VFSReader::browse_block_content(const FSPos& cont_pos, std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f){
		
	if (!this->state && !this->open()){	

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
	}
	else{
		this->raise(3);
	}

	return *this;
}


VFSReader& VFSReader::probe_fsblock(const FSPos& pos, FSBlock& block){
	
	if (!this->state && !this->open()){	
		this->stream->seekg(pos);
		this->stream->read((char*)&block, sizeof(FSBlock));
		AfterReading<FSBlock>::behavior(block);
	}
	else{
		this->raise(4);
	}

	return *this;
}


VFSReader& VFSReader::backtrack(std::vector<FSBlock>& blocks, const FSPos& pos){
	if (!this->state && !this->open()){	
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
	}
	else{
		this->raise(5);
	}

	return *this;
}



VFSReader& VFSReader::fetch_level(const FSBlock& b, std::vector<FSBlock>& blocks, FSize& s_sz){
	if (!this->state && !this->open()){	
		FSize total = 0;

		if (b.content > 0){
			this->browse_block_content(b.content, [&](const FSize& seg_size, const FSBlock* blcs, const FSPos& stt, const FSPos& next){
				for (size_t i = 0 ; i < seg_size ; i++){
					if (!removed_raised(blcs[i].flag)){
						blocks.push_back(blcs[i]);
						total++;
					}
				}
			});
		}
		s_sz = total;
	}
	else{
		this->raise(6);
	}

	return *this;
}


VFSReader& VFSReader::iterative_dfs_hierarchy(const FSPos& start, const std::function<void(const FSBlock&, const size_t&)>& cb){

	if (!this->state && !this->open()){	
		FSBlock current;
		this->sequence(start).read<FSBlock>(current);

		std::vector<FSBlock> stack;
		stack.push_back(current);
		size_t depth = 0;
		FSize seg_size = 0;

		while (stack.size() > 0){
			current = stack.back();
			stack.pop_back();
			
			if (is_undefined(current.flag)){
				depth--;
			}
			else{
				stack.push_back(FSBlock());
				this->fetch_level(current, stack, seg_size);
				cb(current, depth);
				depth++;
			}
		}
	}
	else{
		this->raise(7);
	}

	return *this;
}


VFSReader::VFSReader(VFS_IO& v, std::function<void()> c): vfs_io(v), callback(c){
	this->init();
	if (!fs::exists(this->vfs_io.vfs_path) || !fs::is_regular_file(this->vfs_io.vfs_path)){
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
	if (this->stream){
		if (this->stream->is_open()){
			return 0;
		}
		else{
			this->close();
		}
	}

	this->stream = std::unique_ptr<std::ofstream>(new std::ofstream(this->vfs_io.vfs_path, std::ios::in | std::ios::out | std::ios::binary));

	if (this->stream->is_open()){
		return 0;
	}
	else{
		this->stream = nullptr;
		this->raise(1);
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


VFSWriter& VFSWriter::blank_segments(const FSize& count, FSPos& insert){

	if (this->open() || this->state || !count){
		this->raise(2);
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
	}

	return *this;
}


VFSWriter& VFSWriter::blank_segment(const FSize& count, FSPos& insert){

	if (this->open() || this->state || !count){
		this->raise(3);
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
	}

	return *this;
}


VFSWriter& VFSWriter::write_to_vfs(OutputBuffer& buffer){
	if (this->open() || this->state){
		this->raise(4);
	}
	else{
		buffer.write_to(this->get_stream());
	}
	return *this;
}

/**
 * @param[in] block: Original block owning the newly inserted data
 * @param[in] pos: Position of the original block in the VFS
 * @param[in] insert: Position of the segment to be linked on the VFS.
 * @param[in] editions: Tweaks brought to the block 
 */
VFSWriter& VFSWriter::link_segment(const FSPos& pos, FSBlock& block, const FSPos& insert, const FSBlock& editions){
	if (!this->state){
		FSPos last = 0;
		this->vfs_io.ghost_reader().probe_fsblock(pos, block).browse_block_content(block.content, [&](const FSize& size, const FSBlock* blocks, const FSPos& c_pos, const FSPos& next){
			last = c_pos + (size * sizeof(FSBlock));
		});

		if (!removed_raised(block.flag)){
			block.nb_files += editions.nb_files;
			block.nb_folders += editions.nb_folders;
			block.nb_versionables += editions.nb_versionables;

			if (block.content == 0){
				block.content = insert;
			}
			else{
				this->sequence(last).add<FSPos>(insert);
			}

			this->override<FSBlock>(pos, block);
		}
		else{
			// IMPROVE: [VFS_IO] What to do if the block that we are linking was deleted?
		}
	}

	return *this;
}

/**
 * Both path contain the root of what must be created, and these elements already exist on the disk.
*/
VFSWriter& VFSWriter::replicate_to(const FSBlock& source, const FSBlock& destination, Path path_src, Path path_dst, const FSPos& root_loc){
	// IMPROVE: [VFSWriter] Before starting, check that both paths exist on the system.
	if (!this->state && !this->open()){

		path_src = path_src.parent_path();
		path_dst = path_dst.parent_path();

		std::vector<std::pair<FSBlock, FSBlock>> stack;
		std::vector<FSPos> locations;

		stack.push_back({
			source,
			destination
		});

		locations.push_back(root_loc);

		FSPos head_pos = 0;

		while (stack.size() > 0){
			FSBlock current_src = stack.back().first;
			FSBlock current_dst = stack.back().second;
			FSPos   current_loc = locations.back();

			stack.pop_back();
			locations.pop_back();

			if (is_undefined(current_src.flag)){ // Marker indicating the end of a depth level.
				path_src = path_src.parent_path();
				path_dst = path_dst.parent_path();
			}
			else{
				SystemName sn_src{current_src.id};
				SystemName sn_dst{current_dst.id};

				path_src /= sn_src.c_str();
				path_dst /= sn_dst.c_str();

				stack.push_back({ // Delimiter
					FSBlock(),
					FSBlock()
				});

				// Here, we must acquire the content of the block and instanciate it.
				FSize count = 0;
				this->head(&head_pos);
				OutputBuffer buffer;
				
				this->vfs_io.ghost_reader().browse_block_content(current_src.content, [&](const FSize& seg_size, const FSBlock* blocks, const FSPos& c_pos, const FSPos& next){
					for (size_t i = 0 ; i < seg_size ; i++){
						if (!removed_raised(blocks[i].flag)){
							FSBlock copy = blocks[i];
							copy.content = 0;
							copy.parent = current_loc;
							copy.id.randomize();

							if (file_raised(blocks[i].flag)){
								current_dst.nb_files++;
							}
							else if (folder_raised(blocks[i].flag)){
								current_dst.nb_folders++;
							}
							else{
								current_dst.nb_versionables++;
							}

							stack.push_back({
								blocks[i],
								copy
							});

							locations.push_back(head_pos + sizeof(FSize) + count * sizeof(FSBlock));
							buffer.add<FSBlock>(copy);
							count++;
						}
					}
				});

				buffer.add<FSPos>(0);
				
				this->add<FSize>(count);
				this->write_to_vfs(buffer);

				if (count > 0){
					current_dst.content = head_pos;
					this->override<FSBlock>(current_loc, current_dst);
				}
			}
		}
	}
	else{
		this->raise(12);
	}

	return *this;
}


VFSWriter& VFSWriter::sequence(const FSPos& pos){
	if (this->open() || this->state){
		this->raise(5);
	}
	else{
		this->stream->seekp(pos);
	}
	return *this;
}


VFSWriter::VFSWriter(VFS_IO& p, std::function<void()> c, const bool& reset): vfs_io(p), callback(c){
	this->init();
	if (!fs::exists(this->vfs_io.vfs_path) || reset){
		touch(this->vfs_io.vfs_path);
	}
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

	return VFSReader(*this, [&](){
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
	
	return VFSWriter(*this, [&](){
		this->writers_count--;
	}, reset);
}


VFSReader VFS_IO::ghost_reader(){
	return VFSReader(*this, [&](){});
}


VFSWriter VFS_IO::ghost_writer(const bool& reset){
	return VFSWriter(*this, [&](){}, reset);
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
	SystemName sn{block.id};
	return base_path / sn.c_str();
}

