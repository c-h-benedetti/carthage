#ifndef VFS_IO_OPERATIONS_HPP_INCLUDED
#define VFS_IO_OPERATIONS_HPP_INCLUDED

#include <fstream>
#include <memory>
#include <functional>
#include "InputBuffer.hpp"
#include "OutputBuffer.hpp"
#include "FSBlock.hpp"
#include <vector>


enum VFSError: int{
	OK, // So far so good!
	OPEN_FAIL, // Failed to open an xstream
	INVALID_COUNT, // Requested a segment of size 0 or the insertion of 0 segments
};

// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       VFS READER IMPLEMENTATION                                                             #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


/**
 * Object responsible for reading operations on the VFS.
 * This class is supposed to handle error codes and generate a final status.
 * The callback lambda-expresion links the reader's life to the VFS_IO that issued it.
 */
class VFSReader {

	int state = VFSError::OK; /// Internal state of the object. Any value > 0 indicates a problem.
	std::unique_ptr<std::ifstream> stream = nullptr; /// Dynamic pointer to the current stream
	std::function<void()> callback; /// Callback function, summoned in the destructor

	const Path& vfs_path; /// Proxy reference to the path held by the VFS_IO object.

private: 

	void raise(const VFSError& err){ this->state |= err; }

	/// Opens a new stream.
	int open();

	/// Closes the current stream and setting pointer to nullptr.
	void close();

	/// Initializes functions handling endianness.
	void init();

public:

	/// Reads an object of type T from the current stream at the current position. Meant to be used chained with VFSReader::sequence().
	template <typename T>
	VFSReader& read(T& t);

	/// Moves the head to the position #p. Meant to be used as a chained operation. (vfs.sequence(0).read<A>().read<B>().status();)
	VFSReader& sequence(const FSPos& p);

	/// Accessor on the current internal state code.
	inline int status() const{ return this->state; }

	/// Browses the VFS linearly, segment by segment, ignoring the hierarchy.
	int linear_browsing(std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f);

	/// Browses the content of a Container. #cont_pos is the position of the content (FSBlock::content), so a segment pointer.
	int browse_block_content(const FSPos& cont_pos, std::function<void(const FSize&, const FSBlock*, const FSPos&, const FSPos&)> f);

	/// Reads the FSBlock located at #pos in the VFS. #pos is a block pointer.
	int probe_fsblock(const FSPos& pos, FSBlock& block);

	/// Browses the FSBlock::parent field until the root. Includes the summoner's block (at #pos)
	int backtrack(std::vector<FSBlock>& blocks, const FSPos& pos);

	VFSReader() = delete;
	VFSReader(const Path& p, std::function<void()> c);

	~VFSReader();
};


// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       VFS WRITER IMPLEMENTATION                                                             #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


/**
 * Object responsible for writing operations on the VFS.
 * This class is supposed to handle error codes and generate a final status.
 * The implementation is made in a way that the VFS should always be left in a stable way.
 * The callback lambda-expresion links the writer's life to the VFS_IO that issued it.
 * This class ensures that the requested path will exist by the end of the constructor's execution.
 */
class VFSWriter {

	int state = VFSError::OK; /// Internal state of the object. Any value > 0 indicates a problem.
	std::unique_ptr<std::ofstream> stream; /// Dynamic pointer to the current stream
	std::function<void()> callback; /// Callback function, summoned in the destructor

	const Path& vfs_path; /// Proxy reference to the path held by the VFS_IO object.

private: 

	void raise(const VFSError& err){ this->state |= err; }

	/// Opens a new stream.
	int open();

	/// Closes the current stream and resets the pointer's value.
	void close();

	/// Initializes functions handling endianness.
	void init();

public:

	/// Writes an object of type T at the #pos position in the VFS.
	template <typename T>
	int override(const FSPos& pos, const T& t);

	/// Inserts #count blank segments of size 1 at the end of the VFS.
	int blank_segments(const FSize& count, FSPos& insert);

	/// Inserts a blank segment containing #count FSBlocks at the end of the VFS.
	int blank_segment(const FSize& count, FSPos& insert);

	/// Writes an object of type T in the current stream at the current position. Meant to be used chained with VFSWriter::sequence().
	template <typename T>
	VFSWriter& add(const T& t);

	/// Opens a stream, performs a seekp at #pos and keeps the stream opened. Meant to be used chained.
	VFSWriter& sequence(const FSPos& pos);

	/// Accessor on the current internal state flag.
	inline int status() const{ return this->state; }

	VFSWriter() = delete;
	VFSWriter(const Path& p, std::function<void()> c, const bool& reset=false);

	~VFSWriter();
};



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       VFS IO IMPLEMENTATION                                                                 #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


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



// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
// #                                                                                             #
// #       TEMPLATE BYTE-WISE SWITCH FUNCTION (ENDIANNESS)                                       #
// #                                                                                             #
// # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


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


#include "VFS_IO.tpp"


#endif // VFS_IO_OPERATIONS_HPP_INCLUDED
