#ifndef BASIC_ARRAY_WRAPER_HPP_INCLUDED
#define BASIC_ARRAY_WRAPER_HPP_INCLUDED

template <typename T>
class Array {

	size_t sz;
	T* data = nullptr;

public:

	Array() = default;
	Array(const size_t& s): sz(s), data(new T[s]){}

	inline size_t size() const{ return this->sz; }
	inline T& at(const size_t& i){ return this->data[i]; }
	inline T& operator[](const size_t& i){ return this->at(i); }

	Array(Array& a){
		this->data = a.data;
		this->sz = a.sz;
		a.sz = 0;
		a.data = nullptr;
	}

	Array(Array&& a){
		this->data = a.data;
		this->sz = a.sz;
		a.sz = 0;
		a.data = nullptr;
	}

	Array& operator=(Array& a){
		this->data = a.data;
		this->sz = a.sz;
		a.sz = 0;
		a.data = nullptr;
		return *this;
	}

	Array& operator=(Array&& a){
		this->data = a.data;
		this->sz = a.sz;
		a.sz = 0;
		a.data = nullptr;
		return *this;
	}

	~Array(){
		if (this->data){
			delete[] this->data;
			this->data = nullptr;
		}
	}
};

#endif // BASIC_ARRAY_WRAPER_HPP_INCLUDED