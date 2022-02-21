#include <iostream>
#include <memory>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <chrono>

class A{

public:

	virtual void who() const{
		int k = 3;
		float c = sqrt(sqrt(sqrt(k)));
		c *= k;
	}
};


class B : public A{

public:

	void who() const override{
		int k = 3;
		float c = sqrt(sqrt(sqrt(k)));
		c *= k;
	}
};


class C : public B{

public:

	void who() const override{
		int k = 3;
		float c = sqrt(sqrt(sqrt(k)));
		c *= k;
	}
};


class D{

	std::vector<A> a_s;
	std::vector<B> b_s;
	std::vector<C> c_s;

public:

	D(){
		size_t _A = rand() % 19000;
		size_t _B = rand() % 19000;
		size_t _C = 57000 - (_A + _B);
		this->a_s.resize(_A);
		this->b_s.resize(_B);
		this->c_s.resize(_C);
		std::cout << _A << " " << _B << " " << _C << " ";
	}

	const size_t size() const{
		return this->a_s.size() + this->b_s.size() + this->c_s.size();
	}

	A* content(size_t i){
		if (i < this->a_s.size()){
			return &this->a_s[i];
		}

		i -= this->a_s.size();

		if (i < this->b_s.size()){
			return &this->b_s[i];
		}

		i -= this->b_s.size();

		if (i < this->c_s.size()){
			return &this->c_s[i];
		}

		return nullptr;
	}

	inline A* operator[](size_t i){
		return this->content(i);
	}
};


int main(int argc, char* argv[], char* env[]){

	srand(time(NULL));
	for (size_t k = 0 ; k < 250 ; k++){

		D d{};

		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0 ; i < d.size() ; i++){
			d[i]->who();
		}

		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = duration_cast<std::chrono::microseconds>(stop - start);

		std::cout << duration.count() << std::endl;
	}

	return 0;
}