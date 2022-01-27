

template <size_t N>
size_t Buffer<N>::add(const void* c, const size_t& s){
	size_t length = (this->current_size + s > N) ? (N - this->current_size) : (s);
	memcpy(this->buffer + this->current_size, c, length);
	this->current_size += length;
	return length;
}


template <size_t N>
void Buffer<N>::nullify(){
	this->current_size = 0;
	memset(this->buffer, 0, N);
}



template <size_t N>
Buffer<N>::Buffer(){
	this->current_size = 0;
}


template <size_t N>
Buffer<N>::Buffer(const Buffer<N>& b){
	this->current_size = b.current_size;
	memcpy(this->buffer, b.buffer, b.current_size);
}


template <size_t N>
Buffer<N>::Buffer(const void* c, const size_t& s){
	this->nullify();
	this->add(c, s);
}


template <size_t N>
Buffer<N>& Buffer<N>::operator=(const Buffer<N>& b){
	this->current_size = b.current_size;
	memcpy(this->buffer, b.buffer, b.current_size);
	return *this;
}


template <size_t N>
void Buffer<N>::get_all(void* dest, const size_t& s) const{
	size_t length = (s >= N) ? (N) : (s);
	memcpy(dest, this->buffer, length);
}


template <size_t N>
void Buffer<N>::get(void* dest, const size_t& s) const{
	size_t length = (s >= this->current_size) ? (this->current_size) : (s);
	memcpy(dest, this->buffer, length);
}


template <size_t N>
void Buffer<N>::write_all_to(std::ostream& o) const{
	o.write(this->buffer, N);
}


template <size_t N>
void Buffer<N>::write_to(std::ostream& o) const{
	o.write(this->buffer, this->current_size);
}


template <size_t N>
void Buffer<N>::read_from(std::istream& s){
	size_t length = N - this->current_size;
	if (length){
		s.read(this->buffer + this->current_size, length);
		this->current_size += length;
	}
}

template <size_t N> 
template <typename T>
size_t Buffer<N>::add(const T& data){
	return this->add(&data, sizeof(T));
}