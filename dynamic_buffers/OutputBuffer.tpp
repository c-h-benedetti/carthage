#include <cstring>

template <typename T>
OutputBuffer& OutputBuffer::add(const T& t){
	T copy(t);
	BeforeWriting<T>::behavior(copy);
	const char* as_array = (const char*)&copy;

	for (size_t i = 0 ; i < sizeof(T) ; i++){
		this->data.push_back(as_array[i]);
	}

	return *this;
}
