#ifndef BATCH_MAP_HPP_INCLUDED
#define BATCH_MAP_HPP_INCLUDED

#include <functional>
#include <vector>

template <typename T>
class TypeToIndex{
public:
	const size_t index;
	TypeToIndex(const size_t& i): index(i);
};


template <typename T>
class IndexedData{

	std::vector<T>& data;
	std::vector<FSize> indices;
	std::function<bool(const T& t1, const T& t2)> t1_inf_to_t2;
	std::function<bool(const FSize& f1, const FSize& f2)> comparison = [&](const FSize& f1, const FSize& f2){
		return this->t1_inf_to_t2(this->data[f1], this->data[f2]);
	};

public:

	void update();

	IndexedData() = delete;
	IndexedData(std::vector<T>& vec, const std::function<bool(const T& t1, const T& t2)>& f): data(vec), t1_inf_to_t2(f){
		this->update();
	}
};


template <typename T>
class Map{

protected:

	std::vector<T> data;
	std::vector<IndexedData<T>> sorted_data;

	const TypeToIndex<int> index_int

};


/**
 * !!! Define an interface for the map anyway !!!
 */

#include "Map.tpp"

#endif // BATCH_MAP_HPP_INCLUDED