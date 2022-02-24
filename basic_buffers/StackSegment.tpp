
template <size_t N>
void StackSegment<N>::nullify(){
	memset(this->buffer, 0, N);
}

template <size_t N>
StackSegment<N>::StackSegment(const uint8_t& c){
	memset(this->buffer, c, N);
}

template <size_t N>
StackSegment<N>::StackSegment(const StackSegment<N>& b){
	memcpy(this->buffer, b.buffer, N);
}

template <size_t N>
bool StackSegment<N>::equals(const StackSegment<N>& s2) const{
	size_t i = 0;
	while ((i < N) && (this->buffer[i] == s2.buffer[i])){
		i++;
	}
	return (i == N);
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

template <size_t N>
StackSegment<N>& StackSegment<N>::operator=(const char* str){
	this->override(str, N);
	return *this;
}
