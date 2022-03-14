
template <typename T>
void standard_reading(std::istream& s, std::vector<T>& v, bool clear){
	size_t header[2] = {};
	size_t nb_entries = 0;
	size_t size_entry = 0;

	s.read((char*)header, 2 * sizeof(size_t));
	nb_entries = header[0];
	size_entry   = header[1];

	if (clear){
		v.clear();
	}
	v.reserve(nb_entries);

	size_t extraction = size_entry * nb_entries;
	char* buffer = new char [extraction];

	s.read(buffer, extraction);

	for (size_t i = 0 ; i < nb_entries ; i++){
		v.push_back(T{buffer + i * size_entry});
	}

	delete[] buffer;

}


// [ ] Adress endianness problem: all class should inherit of an interface having operations before_save() and after_load()
//     These two methods will be called over bitwise copies rather than modifying the originals.
