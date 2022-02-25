#include <iostream>
#include <vector>



class A{
	
	std::vector<int>& vec;

public:

	A() = delete;
	A(std::vector<int>& v): vec(v){}

	void tell() const{
		for (const int& i : this->vec){
			std::cout << i << " ";
		}
		std::cout << std::endl;
	}

	virtual void who() const{
		std::cout << "A" << std::endl;
	}

};

class B : public A{

public:

	B(std::vector<int>& v): A(v){}

	void who() const override{
		std::cout << "B" << std::endl;
	}

};

void function(const A* as, const size_t s){
	for (size_t i = 0 ; i < s ; i++){
		as[i].who();
	}
}

void fx(const A& a){

}

class C;

class D{

	C& c;

public:

	D(C& cc): c(cc){}
};

int main(int argc, char* argv[], char* env[]){

	// std::vector<int> mv = {1, 2, 3, 4, 5, 6};
	// std::vector<A> as = {A(mv), A(mv), A(mv), A(mv)};
	// std::vector<B> bs = {B{mv}, B{mv}, B{mv}, B{mv}};

	// function(bs.data(), bs.size());

	return 0;
}