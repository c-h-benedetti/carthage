
template <size_t N>
void StackSegment<N>::nullify(){
	memset(this->buffer, 0, N);
}


template <size_t N>
StackSegment<N>::StackSegment(const StackSegment<N>& b){
	memcpy(this->buffer, b.buffer, N);
}


template <size_t N>
StackSegment<N>::StackSegment(const void* c, const size_t& s){
	this->override(c, s);
}


template <size_t N>
StackSegment<N>& StackSegment<N>::operator=(const StackSegment<N>& b){
	memcpy(this->buffer, b.buffer, N);
	return *this;
}


template <size_t N>
void StackSegment<N>::override(const void* src, const size_t& s){
	this->nullify();
	size_t length = (s > N) ? (N) : (s);
	memcpy(this->buffer, src, length);
}


template <size_t N>
void StackSegment<N>::write_to(std::ostream& o) const{
	o.write((char*)this->buffer, N);
}


template <size_t N>
void StackSegment<N>::read_from(std::istream& s){
	s.read((char*)this->buffer, N);
}


template <size_t N>
void StackSegment<N>::copy(void* dest) const{
	memcpy(dest, this->buffer, N);
}


template <size_t K>
std::ostream& operator<<(std::ostream& o, const StackSegment<K>& s){
	s.write_to(o);
	return o;
}

template <size_t K>
std::istream& operator>>(std::istream& o, StackSegment<K>& s){
	s.read_from(o);
	return o;
}
