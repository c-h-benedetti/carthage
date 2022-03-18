
template <typename T>
VFSReader& VFSReader::read(T& t){
	if (!this->state){
		this->stream->read((char*)&t, sizeof(T));
		AfterReading<T>::behavior(t);
	}
	return *this;
}





template <typename T>
VFSWriter& VFSWriter::add(const T& t){
	if (!this->state){
		T copy = t;
		BeforeWriting<T>::behavior(copy);
		this->stream->write((char*)&copy, sizeof(T));
	}
	return *this;
}


template <typename T>
VFSWriter& VFSWriter::override(const FSPos& pos, const T& t){
	if (!this->state){
		T data = t;
		BeforeWriting<T>::behavior(data);
		this->stream->seekp(pos);
		this->stream->write((char*)&data, sizeof(T));
	}

	return *this;
}