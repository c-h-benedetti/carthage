#include <fstream>

/**
 * Function writing blank segments of size 1 by the end of the VFS to reserve the spot.
 * The purpose of this function is also to acquire an insertion position that will be use in the constructor of an FSObject
 */
template <size_t T>
FSPos Container::blank_segments() const{
	const size_t segment_size = sizeof(FSize) + sizeof(FSBlock) + sizeof(FSPos);
	BasicBuffer<segment_size> blank(0);
	blank.add<FSize>(1);
	BasicBuffer<T*segment_size> total;

	for (size_t i = 0 ; i < T ; i++){
		total.add(blank);
	}

	std::ofstream o_str(this->vfs_path(), std::ios::app | std::ios::binary);

	// IMPROVE: [Container] Must add error handling in blank_segments().
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

template <typename T>
bool name_exists(const Name& n, const Extension& ext, const std::vector<T>& objs){
	size_t i = 0;
	bool found = false;

	while ((i < objs.size()) && !found){
		found = (objs[i].data().name == n) && (objs[i].data().extension == ext);
	}

	if (found){
		// Display a message
	}
	return found;
}

template <typename T, size_t N>
void Container::create_element(
	const Name& n, 
	const Extension& ext, 
	FSBlock& blc, 
	std::function<T(const Path&, const FSPos&)>& instanciating,
	std::vector<T>& collection)
{

	if (name_exists<T>(n, ext, collection)){
		return;
	}
	
	FSPos insert = this->blank_segments<N>();
	Path abs_path = this->make_path(blc);

	T obj = instanciating(abs_path, insert);

	this->refresh_vfs();
	obj.refresh_vfs();
	collection.push_back(obj);
}