#ifndef BATCH_MAP_HPP_INCLUDED
#define BATCH_MAP_HPP_INCLUDED

#include <functional>
#include <vector>

template <class K, class D>
class Map{

	std::vector<K> keys;
	std::vector<D> values;
	std::vector<size_t> indices;
	std::function<bool(const K&, const K&)> comparison; // K1 < K2

public:

	Map() = delete;
	Map(std::vector<K>& k, std::vector<D> d, std::function<bool(const K&, const K&)> c);

	inline size_t size() const{ return this->keys.size(); }
	
	void const_for_each(std::function<void(const size_t&, const K&, const D&)> f) const;
	void for_each(std::function<void(const size_t&, const K&, D&)> f);
};

#include "Map.tpp"

#endif // BATCH_MAP_HPP_INCLUDED