#include <fstream>

/**
 * Function writing blank segments of size 1 by the end of the VFS to reserve the spot.
 * The purpose of this function is also to acquire an insertion position that will be use in the constructor of an FSObject
 * 
 * | Returns the position of insertion of the first segment in case of success.
 * | Returns 0 on failure.
 */
template <size_t T>
FSPos Container::blank_segments() const{
	static_assert(T > 0, "We cannot insert 0 blocks");
	// Create an empty segment of size 1 (size = 1, block = null, next = 0)
	const size_t segment_size = sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);
	BasicBuffer<segment_size> blank(0);
	blank.add<FSize>(1);

	// Creating a buffer of T segments and filling it with T copies of the previously defined blank segment.
	BasicBuffer<T*segment_size> total;
	for (size_t i = 0 ; i < T ; i++){
		total.add(blank);
	}

	// Writing the T new blocks at the end of the VFS.
	std::ofstream o_str(this->vfs_path(), std::ios::app | std::ios::binary);
	if (!o_str.is_open()){
		std::cerr << "Failed to open VFS" << std::endl;
		return 0;
	}

	o_str.seekp(0, std::ios_base::end);
	FSPos insert = o_str.tellp();
	total.write_to(o_str);
	o_str.close();

	return insert;
}


// IMPROVE: [Container] Write return codes of create_element()
/**
 * Description of the process to create a new generic element in a Container.
 */
template <size_t N>
int Container::create_element(FSBlock& blc, std::function<FSObject(const Path&, const FSPos&)>& instanciating,	std::vector<FSObject>& collection){
	// 1. Checks that the name doesn't already exist in this Container
	if (name_exists(blc.name, blc.extension)){
		std::cerr << "The name " << blc.name << " already exists" << std::endl;
		return 1;
	}
	
	// 2. Reserving segment(s) by the end of the VFS that will contain the new elements.
	FSPos insert = this->blank_segments<N>();

	// 3. Building the path according to the data in the new block.
	Path abs_path = this->make_path(blc);

	// 4. Executing operations specific to a type of objects (like creating the block).
	FSObject obj = instanciating(abs_path, insert);

	// 5. Writing blocks into the VFS

	int code = 0;

	// IMPROVE: [Container] Handle linking error codes.
	this->linking_segment(insert);
	collection.push_back(obj);

	if (this->override_vfs()){
		std::cerr << "Failed to update VFS to override this" << std::endl;
		code |= (1 << 1);
	}

	if (obj.override_vfs()){
		std::cerr << "Failed to update VFS to override new object's segment" << std::endl;
		code |= (1 << 2);
	}

	return code;
}