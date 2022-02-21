

template <size_t N>
void BasicBuffer<N>::clear(){
	this->nullify();
	this->pos = 0;
}


template <size_t N>
BasicBuffer<N>& BasicBuffer<N>::copy_from(const void* src){
	size_t len = strlen((const char*)src);
	memcpy(this->buffer + this->pos, src, len);
	this->pos += len;
	return *this;
}


template <size_t N>
BasicBuffer<N>& BasicBuffer<N>::copy_from(const void* src, const size_t& sz){
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

// = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

template <size_t K>
BufferReader<K>::BufferReader(const BasicBuffer<K>& buf): buffer(&buf){}

template <size_t K> template <typename T>
BufferReader<K>& BufferReader<K>::get(T& t){
	memcpy(&t, this->buffer->data() + this->pos, sizeof(T));
	this->pos += sizeof(T);
	return *this;
}