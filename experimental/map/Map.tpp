#include <utility>
#include <algorithm>
#include <numeric>

template <class K, class D>
Map<K, D>::Map(std::vector<K>& k, std::vector<D> d, std::function<bool(const K&, const K&)> c): keys(std::move(k)), values(std::move(d)), comparison(c){
	this->indices.clear();
	this->indices.resize(this->keys.size());
	std::iota(this->indices.begin(), this->indices.end(), 0);

	// i1 < i2
	std::function<bool(const size_t&, const size_t&)> f = [&](const size_t& i1, const size_t& i2){
		return this->comparison(this->keys[i1], this->keys[i2]);
	};

	std::sort(this->indices.begin(), this->indices.end(), f);
}

template <class K, class D>
void Map<K, D>::const_for_each(std::function<void(const size_t&, const K&, const D&)> f) const{
	for(size_t i = 0 ; i < this->keys.size() ; i++){
		f(
			i, 
			this->keys[this->indices[i]],
			this->values[this->indices[i]]
		);
	}
}

template <class K, class D>
void Map<K, D>::for_each(std::function<void(const size_t&, const K&, D&)> f){
	for(size_t i = 0 ; i < this->keys.size() ; i++){
		f(
			i, 
			this->keys[this->indices[i]],
			this->values[this->indices[i]]
		);
	}
}