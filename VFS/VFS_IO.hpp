#ifndef VFS_IO_OPERATIONS_HPP_INCLUDED
#define VFS_IO_OPERATIONS_HPP_INCLUDED

#include <fstream>
#include <memory>
#include <functional>
#include "InputBuffer.hpp"
#include "OutputBuffer.hpp"
#include "FSBlock.hpp"
#include <vector>

// IMPROVE: [VFSWriter][VFSReader] The embeded status should take its values from an enum class.
// IMPROVE: [VFSWriter][VFSReader] The value of the status should always be check before any operation. Abort it if necessary.
// IMPROVE: [VFSWriter][VFSReader] The buffer should be an object instanciated in the local scope of methods, it should be removed from the instance scope.
// IMPROVE: [VFSWriter][VFSReader] The open() methods should be summoned from the constructor, the close() method from the destructor.

/**
 * Object responsible for reading operations on the VFS.
 * This class is supposed to handle error codes and generate a final status.
 * The implementation is made in a way that the VFS should always be left in a stable way.
 * The callback lambda-expresion links the reader's life to the VFS_IO that issued it.
 */
class VFSReader {

	int state = 0;
	InputBuffer buffer;
	std::unique_ptr<std::ifstream> stream;
	std::function<void()> callback;

	const Path& vfs_path;

private: 

	int open();
	int close();
	void init();

	inline std::ifstream& get_stream(){
		return *(this->stream.get());
	}

public:

	// IMPROVE: [VFSWriter][VFSReader] These functions can be refactorized into a unique one with a template type.
	// Opens the VFS and moves the head to the position #p. Meant to be used as a chained operation. (vfs.sequence(0).read_xxx().read_xxx().status();)
	VFSReader& sequence(const FSPos& p);
	VFSReader& read_fsize(FSize& s);
	VFSReader& read_fsblock(FSBlock& b);
	VFSReader& read_fspos(FSPos& p);
	inline int status() const{ return this->state; }

	/// Browses the VFS linearly segment by segment, ignoring the hierarchy.
	int linear_browsing(std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f);

	/// Browses the content of a Container. #cont_pos is the position of the content (FSBlock::content).
	int browse_block_content(const FSPos& cont_pos, std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f);

	/// Reads the FSBlock located at #pos in the VFS.
	int probe_fsblock(const FSPos& pos, FSBlock& block);

	/// Browses the FSBlock::parent field until the root. Includes the summoner's block (at pos)
	int backtrack(std::vector<FSBlock>& blocks, const FSPos& pos);

	VFSReader() = delete;
	VFSReader(const Path& p, std::function<void()> c);

	~VFSReader();
};


// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +

class VFSWriter {

	int state = 0;
	OutputBuffer buffer;
	std::unique_ptr<std::ofstream> stream;
	std::function<void()> callback;

	const Path& vfs_path;

private: 

	int open();
	int close();
	void init();

	inline std::ofstream& get_stream(){
		return *(this->stream.get());
	}

public:

	int override_fsblock(const FSPos& pos, const FSBlock& block);
	// Writes the address #next_val after doing a seekp(pos)
	int override_address(const FSPos& pos, const FSPos& next_val);
	int override_fsize(const FSPos& pos, const FSize& size);

	/// Inserts #count blank segments of size 1 by the end of the VFS.
	int blank_segments(const FSize& count, FSPos& insert);
	/// Inserts a blank segment containing #count FSBlocks by the end of the VFS.
	int blank_segment(const FSize& count, FSPos& insert);


	VFSWriter& sequence(const FSPos& pos);
	VFSWriter& add_fsize(const FSize& size);
	VFSWriter& add_fsblock(const FSBlock& blc);
	VFSWriter& add_fspos(const FSPos& pos);
	inline int status() const{ return this->state; }

	VFSWriter() = delete;
	VFSWriter(const Path& p, std::function<void()> c, const bool& reset=false);

	~VFSWriter();
};


// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +


template <typename A>
void mirror_bytes(A& a){
	char* bytes = (char*) &a;
	char res[sizeof(A)];

	for (size_t i = 0 ; i < sizeof(A) ; i++){
		res[i] = bytes[i];
	}

	for (size_t i = 0 ; i < sizeof(A) ; i++){
		bytes[i] = res[sizeof(A)-i];
	}
}


// + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + +


class VFS_IO {

	size_t readers_count = 0;
	size_t writers_count = 0;

public:

	const Path user_root;
	const Path vfs_path;

public:

	VFSReader ask_reader();
	VFSWriter ask_writer(const bool& reset=false);

	// Concatenation of base_path and the name built from the block
	Path make_path(const Path& base_path, FSBlock& block);

	VFS_IO(const Path& u_root);
};

#endif // VFS_IO_OPERATIONS_HPP_INCLUDED