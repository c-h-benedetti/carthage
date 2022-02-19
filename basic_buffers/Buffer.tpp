

template <size_t N>
void BasicBuffer<N>::clear(){
	this->nullify();
	this->pos = 0;
}


template <size_t N>
BasicBuffer<N>& BasicBuffer<N>::add(const void* src){
	size_t len = strlen(src);
	memcpy(this->buffer + this->pos, src, len);
	this->pos += len;
	return *this;
}


template <size_t N>
BasicBuffer<N>& BasicBuffer<N>::add(const void* src, const size_t& sz){
	size_t len = sz;
	memcpy(this->buffer + this->pos, src, len);
	this->pos += len;
	return *this;
}


template <size_t N>
template <typename T>
BasicBuffer<N>& BasicBuffer<N>::add(const T& t){
	size_t len = sizeof(t);
	memcpy(this->buffer + this->pos, &t, len);
	this->pos += len;
	return *this;
}

