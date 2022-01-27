

### Notes

- This project must be compiled with C++20.
- *Carthage* is aimed to be a stateless server to reduce its weight on users' machines.
- As these lines are written, *Carthage* is implemented "in the best case", a very few errors are handled, overall the ones coming from wrong users' inputs.

### 0. General

- [ ] The IO descriptor should all be closed when we launch the software. All the `tell()` operations are for debugging purposes only.
- [ ] The variable `#define PROJECTS_FILE`(Project.cpp) should be acquired from the environment instead of being hard coded as preprocessor variable. However, it implies one more check when we initialize the software.
- [ ] Use the new function `copy_to` instead of `memcpy` with additions, to make things cleaner.
- [ ] To clean things up, the Buffer should be a class by itself. But doing that keeps us from doing a stack-only implementation...

```cpp
class Buffer{

	size_t head;
	size_t size;

};
```

### 1. Projects & Project

- [ ] Load all projects (not just valid ones) and rewrite the whole file in case of edition. It is still more effective than placing a `seekp` and shifting the rest of the document.
	- In case of addition, we rewrite everything after the new Project was added to the map. 
	- In case of deletion, we just have to skip the user-provided ID while we write.
	- To avoid code duplication of the saving operation, we could add a variable inside projects indicating whether they have to be skipped for all operations, this same variable could be used to represent the validity of the project.
- [ ] The `get_project(const ID& id)` operation should return a pointer to handle the case where the provided ID is not valid or is not contained in the structure.
- [ ] Make paths have a maximal size, and pad them with 0 when we write files. In a such way, we can read and write with a buffer of fixed size in only one extraction.
- [ ] Try to find a cleaner way to pack/unpack to/from buffers.
- [ ] Instead of storing the color in the project's attribute, we could instead sum all the letters of the name and take the modulo `k ∈ ℕ*`.

### 2. FileSystem, File, Folder, Versionable

- [ ] Write an empty constructor for all these objects for the code to compile.
- [ ] Add a `Segment` class to optimize the writting operations, and bunch as much data as possible in Segments

```cpp
class Segment{

std::vector<FSBlock> blocks;

public:

	Segment& add_file(...);
    Segment& add_folder(...);
    Segment& add_versionable(...);
    int write();
};

class Folder{
		...
	Segment new_segment();
    	...
};
```

- [ ] Test if the `Table` object is stable and can be added to the project.
- [ ] Write the methods that enable to write in the descriptor.


When we want to write some content in a Folder, there are two cases. The Folder was empty so far, or some content may already be present.

- In this first case, the `content_address` variable from the `FSBlock` has a value of `0`. We just have to overwrite this `0` with the size of the descriptor before we write the segment in the descriptor.
- In this case, we must jump to the position of `content_address` and browse the segments until we find a `0` as next segment's position.


- [ ] Test if the overwritting works correctly (no document loss). Also, check if the IO operations couldn't be more optimized, a lot of `seekp` and `seekg` are required with the current implementation.


```cpp
// = = = READING = = =

template <typename T>
void copy_to(void* source, void* dest, const T& size, T& place){
	memcpy(dest + place, source, size);
	place += size;
}

// We suppose that the Folder object was already read to fill its block attribute
// We could add to the Folder an attribute representing the position of the last segment's end
//  => FPos next_address_position = 0;

void Segment::write(const Folder& target){ 

	FSize size = this->blocks.size();
	FSize pos = 0;
	FSize size_buffer = sizeof(FSize) + this->blocks.size() * sizeof(FSBlock) + sizeof(FPos);
	buffer[size_buffer] = {};

	copy_to<FSize>(&size, buffer, sizeof(FSize), pos);
	for (FSize i = 0 ; i < size ; i++){
		copy_to<FSize>(this->blocks + i, buffer, sizeof(FSBlock), pos);
	}


	file = ofstream(descriptor_path, binary | out | app);
	file.seekp(std::ios_base::end);
	FPos end_f = file.tellp();

	if (target.content_address){
		file.seekp(target.next_address_position);
	}
	else{
		// content_address is the first variable of the block, so no shifting is required
		file.seekp(target.fpos);
	}

	file.write((char*)&end_f, sizeof(FPos));
	file.seekp(std::ios_base::end);
	file.write(buffer, size_buffer);

	file.close();
}

```

### 3. Tables

- Since we are working on a stateless server, we want to be able to access any VFS object by its ID.
- Carthage is built to handle extremely big projects, with possibly several millions of assets, and many versions for each of them.
- We don't want to load in RAM the whole table associating an ID to a FPos because it could be several GB of weight.
- The Table class was created to reduce the RAM weight of tables, and avoid copying the whole table to keep data sorted.

- Tables are splitted in an undetermined number of files. A file represents a range of IDs, and has a limited number of entries.
- A range has a limited number of entries but the size of the range is not predefined. If we need to insert some data in a table which already reached its size of entries, the table is bisected before the insertion.


### 4. Subprocess

- Only implemented for Posix machines right now.


##### Notes:

- `std::vector<A> vec;` requires `A` to be a fully defined class when we declare `vec`. With the multiples cross inclusions between all objects of the file system, it is impossible to use a `std::vector` to store the content of a `Folder`. Native heap segment were used instead, even though it is not what "modern C++" wants in this context.
- In order to use `operator[]` on a `std::map<A, B>`, the `B` type must implement an empty constructor. Otherwise, we can only use `my_map.find(my_A_obj)->second`. In both case we must handle the possible absence of our key in our collection.