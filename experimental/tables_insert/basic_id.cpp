#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <ctime>
#include <cstring>
#include <vector>
#include <utility>
#include <algorithm>
#include "ID.hpp"

namespace fs = std::filesystem;
using String = std::string;
using Path = fs::path;
using FilePos = uint64_t;

using TableSize = uint16_t;
#define TABLE_MAX_SIZE 100



class Entry{

public:

	ID first = ID::null_id();
	FilePos second = 0;

	Entry() = default;
	Entry(const char* raw_data){
		this->first = ID(raw_data);
		memcpy(&(this->second), raw_data+SIZE_ID, sizeof(FilePos));
	}

	Entry(const Entry& e): first(e.first), second(e.second){}

	Entry(const ID& id, const FilePos& fp): first(id), second(fp){}

	void raw_data(char* buffer) const{
		this->first.get(buffer);
		memcpy(buffer+SIZE_ID, &this->second, sizeof(FilePos));
	}

};

bool operator<(const Entry& t1, const Entry& t2){
	return t1.first < t2.first;
}

bool operator>(const Entry& t1, const Entry& t2){
	return t1.first > t2.first;
}

bool operator==(const Entry& t1, const Entry& t2){
	return t1.first == t2.first;
}

bool operator<=(const Entry& t1, const Entry& t2){
	return t1.first <= t2.first;
}

bool operator>=(const Entry& t1, const Entry& t2){
	return t1.first >= t2.first;
}



class SubTable{

	std::vector<Entry>* content = nullptr;

	// vvv These attributes must have their value whether the subtable is loaded or not vvv
	Path name;
	ID range_start;
	ID range_end;
	TableSize size = 0;

	SubTable(const Path& n, const ID& r1, const ID& r2, const TableSize& ts): name(n), range_start(r1), range_end(r2), size(ts){}

	void set_vector(std::vector<Entry>* c){
		this->content = c;
	}

public:

	inline size_t get_size() const{return this->size;}

	SubTable empty_clone() const{
		return SubTable(this->name, this->range_start, this->range_end, this->size);
	}

	TableSize find(const ID& id){
		std::vector<Entry>& c = *(this->content);
		int64_t size = c.size();
		int64_t first = 0;
		int64_t last = size - 1;
		int64_t mid = 0;

		// Returns the index of the desired element, or the index of the biggest element smaller than the target
		// if the target is not in the vector. Return value in [-1, size-1]

		if (id < c[0].first){
			return 0;
		}
		else if (id > c[last].first){
			return size;
		}
		else{
			while (first <= last){
				mid = (first + last) / 2;
				if (id > c[mid].first){
					first = mid + 1;
				}
				else{
					last = mid - 1;
				}
			}
			return first;
		}
	}

	void insert(const ID& id, const FilePos& fp){
		if (this->content->size() == 0){
			this->content->push_back(Entry(id, fp));
		}
		else{
			TableSize target = this->find(id);
			this->content->insert(this->content->begin() + target, Entry(id, fp));


			if (target == 0){
				this->range_start = id;
			}
			else if (target == this->size){
				this->range_end = id;
			}
		}
		this->size++;
	}

	void load(){
		if (!this->is_loaded()){
			this->content = new std::vector<Entry>;
			if (fs::exists(this->name)){
				std::ifstream stream(this->name.c_str(), std::ios::in | std::ios::binary);
				if (stream.is_open()){
					uint16_t buffer_size = SIZE_ID + sizeof(FilePos);
					char buffer[buffer_size];
					while (stream.read(buffer, buffer_size)){
						this->content->push_back(Entry(buffer));
					}
					stream.close();
				}
			}
		}
	}

	SubTable bisect(const Path& p, const ID& key, uint64_t& target){
		// Index of the first item of the second half

		uint64_t mid = this->content->size() / 2;
		size_t sz_second = this->content->size() - mid;

		if (key >= this->content->at(mid).first){
			// We are currently creating a table that will be inserted after 'target'
			// So it's safe to use the index after 'target'
			target++;
		}

		std::vector<Entry>* c = new std::vector<Entry>{this->content->begin()+mid, this->content->end()};

		char bfr[SIZE_ID+1] = {};
		build_name(bfr);

		SubTable tb(
			p / bfr,
			this->content->at(mid).first,
			this->content->back().first,
			sz_second
		);
		tb.set_vector(c);
		this->range_end = this->content->at(mid-1).first;

		std::vector<Entry>::iterator it = this->content->begin() + mid;
		this->content->erase(it, this->content->end());

		this->size = mid;

		return tb;
	}

	void write(){
		std::ofstream stream(this->name, std::ios::out | std::ios::binary);
		if (stream.is_open()){
			uint16_t buffer_size = SIZE_ID + sizeof(FilePos);
			char buffer[buffer_size];
			
			for (TableSize i = 0 ; i < this->content->size() ; i++){
				this->content->at(i).raw_data(buffer);
				stream.write(buffer, buffer_size);
			}

			stream.close();
		}
	}

	void unload(){
		delete this->content;
		this->content = nullptr;
	}

	inline bool is_loaded() const{return this->content != nullptr;}
	inline bool is_opened() const{return this->size < TABLE_MAX_SIZE;}

	SubTable() = delete;

	SubTable(const Path& p, const char* raw_data): range_start(raw_data+SIZE_ID), range_end(raw_data+SIZE_ID+SIZE_ID){
		char buffer[SIZE_ID+1] = {};
		memcpy(buffer, raw_data, SIZE_ID);
		this->name = p / buffer;
		memcpy(&(this->size), raw_data+SIZE_ID+SIZE_ID+SIZE_ID, sizeof(TableSize));
	}


	friend bool operator<(const SubTable& t1, const SubTable& t2);
	friend bool operator>(const SubTable& t1, const SubTable& t2);
	friend bool operator==(const SubTable& t1, const SubTable& t2);
	friend bool operator<=(const SubTable& t1, const SubTable& t2);
	friend bool operator>=(const SubTable& t1, const SubTable& t2);

	friend bool operator<(const ID& t1, const SubTable& t2);
	friend bool operator>(const ID& t1, const SubTable& t2);

	friend class Table;

};

bool operator<(const SubTable& t1, const SubTable& t2){
	return t1.range_end < t2.range_start;
}

bool operator>(const SubTable& t1, const SubTable& t2){
	return t1.range_end > t2.range_start;
}

bool operator==(const SubTable& t1, const SubTable& t2){
	return t1.range_end == t2.range_start;
}

bool operator<=(const SubTable& t1, const SubTable& t2){
	return t1.range_end <= t2.range_start;
}

bool operator>=(const SubTable& t1, const SubTable& t2){
	return t1.range_end >= t2.range_start;
}



bool operator<(const ID& t1, const SubTable& t2){
	return t1 < t2.range_start;
}

bool operator>(const ID& t1, const SubTable& t2){
	return t1 > t2.range_end;
}




class Table{

	Path path;
	Path name;
	Path full_path;
	std::vector<SubTable> tables;
	uint64_t active = 0;

public:

	Table() = delete; // OK

	Table(const Path& p, const Path& n) : path(p), name(n), full_path(p / n){ // OK
		if (fs::exists(this->full_path)){
			std::ifstream stream(this->full_path.c_str(), std::ios::in | std::ios::binary);
			if (stream.is_open()){
				uint16_t buffer_size = SIZE_ID + SIZE_ID + SIZE_ID + sizeof(TableSize);
				char buffer[buffer_size];
				while (stream.read(buffer, buffer_size)){
					this->tables.push_back(SubTable(this->path, buffer));
				}
				stream.close();
			}
		}
	}


	~Table(){ // OK
		for (SubTable& t : this->tables){
			if (t.is_loaded()){
				t.write();
				t.unload();
			}
		}
		this->export_tables();
	}


	TableSize find_table(const ID& id) const{ // OK if this->tables is not empty

		size_t size = this->tables.size();
		int64_t first = 0;
		int64_t last = size - 1;
		int64_t mid = 0;

		while (first <= last){
			mid = (first + last) / 2;
			if (id < this->tables[mid]){
				last = mid - 1;
			}
			else{
				first = mid + 1;
			}
		}

		if (last < 0){
			return 0;
		}
		else if (last >= static_cast<int64_t>(size)){
			return (size - 1);
		}
		else{
			return last;
		}
	}


	void tell(){
		for (SubTable& t : this->tables){
			std::cout << "===========" << std::endl;
			std::cout << t.name << std::endl;
			t.range_start.tell();
			t.range_end.tell();
			t.load();
			for (const Entry& e : *(t.content)){
				std::cout << ">>> ";
				e.first.tell();
			}
			t.unload();
			std::cout << t.size << std::endl;
			std::cout << "===========" << std::endl;
		}
	}


	void export_tables(){ // OK
		std::ofstream stream(this->full_path.c_str(), std::ios::out | std::ios::binary);
		
		if (stream.is_open()){
			uint16_t buffer_size = SIZE_ID + SIZE_ID + SIZE_ID + sizeof(TableSize);
			char buffer[buffer_size];

			for (const SubTable& t : this->tables){
				memcpy(buffer, t.name.filename().c_str(), SIZE_ID);
				t.range_start.get(buffer + SIZE_ID);
				t.range_end.get(buffer + SIZE_ID + SIZE_ID);
				memcpy(buffer + SIZE_ID + SIZE_ID + SIZE_ID, &(t.size), sizeof(TableSize));
				stream.write(buffer, buffer_size);
			}

			stream.close();
		}
	}


	void add(const ID& id, const FilePos& fp){ // Looks OK

		if (this->tables.size() > 0){
			TableSize target = this->find_table(id);

			if ((target != this->active) && (this->tables[this->active].is_loaded())){
				this->tables[this->active].write();
				this->tables[this->active].unload();
				this->active = target;
			}

			this->tables[this->active].load();

			if (!this->tables[this->active].is_opened()){
				uint64_t next = this->active;

				this->tables.insert(this->tables.begin() + this->active + 1, this->tables[this->active].bisect(this->path, id, next));

				if (this->active != next){
					this->tables[this->active].write();
					this->tables[this->active].unload();
					this->active = next;
					this->tables[this->active].load();
				}
			}

			this->tables[this->active].insert(id, fp);

		}
		else{
			char buffer[SIZE_ID+1] = {};
			build_name(buffer);

			this->tables.push_back(
				SubTable(
					this->path / buffer, 
					id, 
					id, 
					0
				)
			);

			this->tables[0].load();
			this->tables[0].insert(id, fp);
		}
	}


	FilePos locate_id(const ID& id){
		if (this->tables.size() == 0){
			return 0;
		}
		else{

			TableSize target = this->find_table(id);

			if ((id >= this->tables[target].range_start) && (id <= this->tables[target].range_end)){
				SubTable table = this->tables[target].empty_clone();
				table.load();
				TableSize pos = table.find(id);
				FilePos fp = 0;

				if ((pos < table.size) && (table.content->at(pos).first == id)){
					fp = table.content->at(pos).second;
				}

				table.unload();
				return fp;
			}
			else{
				return 0;
			}
		}
	}

};

int main(int argc, char* argv[], char* env[]){

	srand(time(NULL));

	Table t1{"testing", "test_table"};
	
	for (size_t i = 0 ; i < 100 ; i++){
		t1.add(ID{}, 42);
	}

	if (t1.locate_id(ID{"h27o1OcH29TolBYRcD3LCSuMnjs47Hi3"})){
		std::cout << "FOUND" << std::endl;
	}

	// t1.tell(); // Le tell empeche l'écriture de la table à cause des unload

	return 0;
}

/*
		~ TODO ~

[!] Make a debugging mode with assertion in preprocessor blocks.
[!] Check each function knowing the different states of our objects.
[!] Check for possible optimizations.
[X] Make the functions that enables to locate something through this system.
[!] Pass this code to a few Valgrind sessions.
[!] Maybe that some space can be reserved in files (with 0) so we can do insertions instead of writting everything again each time
	We reserve the double of the size each time (like vectors)
[!] Time the program and identify bottlenecks (LOL)
[!] Separate the code in several files.
[!] Make an assessment in the notebook
[!] The locate_id() is stateless, the add() is not and requires memory management => Secure the add() with a mutex.
[!] Compare execution time for different sizes of tables. Try to time with and without the file reserve
[X] Inspect possible states of both the dichotomy algorithm
[!] Try to make a tell operation that doesn't interfer with the state of the class
[!] Search for the most optimized sizes




		~ Notes & Observations ~

- For 150K random items to insert, the program is faster for tables of size 2.5K that 25K

*/