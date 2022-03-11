#include <iostream>
#include <functional>

class A {

	std::function<void()> cb;

public:

	A(std::function<void()> c): cb(c){}

	~A(){
		this->cb();
	}
};

class B {

	size_t count = 0;

public:

	void tell() const{
		std::cout << this->count << std::endl;
	}


	A new_a(){

		this->count ++;

		return A([&](){
			this->count--;
		});
	}

};

void f(B& b){
	b.tell();
	A a = b.new_a();
	b.tell();
}

int main(){

	B b;
	f(b);
	b.tell();

	return 0;
}