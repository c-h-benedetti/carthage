#include <cstring>

template <typename T>
InputBuffer& InputBuffer::get(T& t){
	memcpy(&t, this->data.data()+this->head, sizeof(T));
	this->head += sizeof(T);
	AfterReading<T>::behavior(t);

	return *this;
}