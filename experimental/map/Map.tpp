#include <utility>
#include <algorithm>
#include <numeric>

template <typename T>
void IndexedData<T>::update(){
	this->indices.clear();
	this->indices.resize(this->data.size());
	std::iota(this->indices.begin(), this->indices.end(), 0);
	std::sort(this->indices.begin(), this->indices.end(), this->comparison);
}
