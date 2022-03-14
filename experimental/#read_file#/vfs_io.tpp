
template <typename T>
FSize VFSWriter::write(const T& t){
	if (this->stream->is_open()){
		try{
			this->stream->write((char*)&t, sizeof(T));
		}
		catch(std::exception& e){
			this->corrupted = true;
			return 0;
		}
		return sizeof(T);
	}
	else{
		return 0;
	}
}