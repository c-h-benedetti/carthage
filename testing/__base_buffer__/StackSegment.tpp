

template <size_t N>
StackSegment<N>& StackSegment<N>::add(const void* c, const size_t& s){
	size_t length = (this->current_size + s > N) ? (N - this->current_size) : (s);
	memcpy(this->buffer + this->current_size, c, length);
	this->current_size += length;
	return (*this);
}


template <size_t N>
void StackSegment<N>::nullify(){
	this->current_size = 0;
	memset(this->buffer, 0, N);
}


template <size_t N>
StackSegment<N>::StackSegment(){
	this->current_size = 0;
}


template <size_t N>
StackSegment<N>::StackSegment(const StackSegment<N>& b){
	this->current_size = b.current_size;
	memcpy(this->buffer, b.buffer, b.current_size);
}


template <size_t N>
StackSegment<N>::StackSegment(const void* c, const size_t& s){
	this->nullify();
	this->add(c, s);
}


template <size_t N>
StackSegment<N>& StackSegment<N>::operator=(const StackSegment<N>& b){
	this->current_size = b.current_size;
	memcpy(this->buffer, b.buffer, b.current_size);
	return *this;
}


template <size_t N>
void StackSegment<N>::override(void* src, const size_t& s) const{
	this->nullify();
	size_t length = (s > N) ? (N) : (s);
	memcpy(this->buffer, src, length);
}


template <size_t N>
void StackSegment<N>::write_all_to(std::ostream& o) const{
	o.write((char*)this->buffer, N);
}


template <size_t N>
void StackSegment<N>::write_to(std::ostream& o) const{
	o.write((char*)this->buffer, this->current_size);
}


template <size_t N>
void StackSegment<N>::read_from(std::istream& s){
	size_t length = N - this->current_size;
	if (length){
		s.read((char*)this->buffer + this->current_size, length);
		this->current_size += length;
	}
}

template <size_t N> 
template <typename T>
StackSegment<N>& StackSegment<N>::add(const T& data){
	return this->add(&data, sizeof(T));
}


template <size_t N>
void StackSegment<N>::copy(void* dest) const{
	memcpy(dest, this->buffer, this->current_size);
}


template <size_t N>
void StackSegment<N>::copy_all(void* dest) const{
	memcpy(dest, this->buffer, N);
}