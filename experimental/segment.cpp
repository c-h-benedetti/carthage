#include <iostream>
#include <cstring>
#include <cinttypes>
#include <vector>

// La méthode get_folder() ne doit pas renvoyer un objet qui soit loaded, le loading doit être explicitement spécifié

// Projects.get_folder("the_project's_id").new_segment().add_folder("some_name_1").add_folder("some_name_2").add_file("some_name_3").write();

// Folder get_folder() renvoie un objet qui connait une fpos et son owner
// Segment new_segment() renvoie un Segment qui connait l'objet Folder
// Chaque Segment& add_object() function add something to the vector

// WRITTING

/*void Segment::write(){

}*/

#define SIZE 10

class A{ // Represents a Block

	char _a[SIZE] = {};

public:

	A() = default;

	A(const char* c){
		memcpy(this->_a, c, SIZE-1);
	}

	A(const A& a){
		memcpy(this->_a, a._a, SIZE-1);
	}

	friend std::ostream& operator<< (std::ostream& os, const A& c);
	friend class Segment;
};

std::ostream& operator<< (std::ostream& os, const A& c){
	os << c._a;
	return os;
}




class Folder;

class Segment{

	std::vector<A> object;

public:

	Folder* f = nullptr;

	Segment& add_object(const char* c){
		this->object.push_back(A{c});
		return *this;
	}

	void tell() const{
		for (const A& a : this->object){
			std::cout << a._a << std::endl;
		}
	}

	Segment(Folder* _f): f(_f){}
	Segment(){}
};




class Folder{

public:

	uint64_t fpos = 42;

	Segment new_segment(){
		return Segment{this};
	}


	Folder() = default;

	// It would be great to block these methods but they will certainly be required at some point
	// so they will very likely be private

	Folder(const Folder& f) = delete;
	Folder(Folder&& f) = delete;

	Folder& operator=(const Folder& f) = delete;
	Folder& operator=(Folder&& f) = delete;
};




class Project{

public:

	Folder get_folder(){
		return Folder{};
	}

};





int main(int argc, char* argv[], char* env[]){

	Project project{};
	project.get_folder().new_segment().add_object("abcdefghij").add_object("cndjwpmrkt").add_object("pokemonpokemon").tell();

	return 0;
}