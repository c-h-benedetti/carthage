#include <iostream>
#include <string>
#include <vector>

enum class State;
inline void transition(char c);
inline bool separator(char c);

inline bool valid_char(char c){
	return ((c >= 64) && (c <= 126)) ||
		   ((c >= 48) && (c <= 57))  ||
		   ((c >= 32) && (c <= 33))  ||
		   ((c >= 35) && (c <= 46))  ||
		   (c == 59) || (c == 61);
}

class Match{
	public:
		size_t p_start = 0;
		size_t p_end = 0;
};

std::ostream& operator<<(std::ostream& o, const Match& m){
	o << "(" << m.p_start << " -> " << m.p_end << ")";
	return o;
}

Match match{};

#if defined(WIN32) || defined(_WIN32)
/*
#define PATH_SEPARATOR '\\'


enum class State{
	START,
	STATE1,
	STATE2,
	STATE3,
	MATCH
};

State current = State::START;

void transition(char c){
	switch(current){
		case(State::START):
			break;
		case(State::STATE1):
			break;
		case(State::STATE2):
			break;
		case(State::STATE3):
			break;
		case(State::MATCH):
			break;
	};
}
*/
#else 

#define PATH_SEPARATOR '/'

enum class State{
	START,
	STATE1,
	MATCH
};

State current = State::START;
size_t position = 0;
std::vector<Match> matches = {};

inline void set_state(State s){
	switch(s){
		case (State::START):
			if (current == State::STATE1){
				matches.push_back(match);
			}
			if (current == State::MATCH){
				matches.push_back(match);
			}
			current = State::START;
			break;

		case (State::STATE1):
			current = State::STATE1;
			break;

		case (State::MATCH):
			if (current == State::START){
				match.p_start = position;
				match.p_end = position + 1;
			}
			else if (current == State::MATCH){
				matches.push_back(match);
				match.p_start = position;
				match.p_end = position + 1;
			}
			else if (current == State::STATE1){
				match.p_end = position + 1;
			}
			current = State::MATCH;
			break;
	};
	position++;
}

void transition(char c){
	switch(current){
		case(State::START):
			if (separator(c)){
				set_state(State::MATCH);
			}
			else{
				set_state(State::START);	
			}
			break;

		case(State::STATE1):
			if (valid_char(c)){
				set_state(State::STATE1);
			}
			else if (separator(c)){
				set_state(State::MATCH);
			}
			else{
				set_state(State::START);
			}
			break;

		case(State::MATCH):
			if (separator(c)){
				set_state(State::MATCH);
			}
			else if (valid_char(c)){
				set_state(State::STATE1);
			}
			else{
				set_state(State::START);
			}
			break;
	};
}

void end_of_stream(){
	if (current == State::MATCH){
		matches.push_back(match);
	}
	else if (current == State::STATE1){
		matches.push_back(match);
	}
}

#endif 

inline bool separator(char c){
	return c == PATH_SEPARATOR;
}

int main(int argc, char* argv[], char* env[]){

	std::string test_path = "//ta/?race tu me connais pas/debut/autre/putain</sarace//sdf";
	position = 0;
	std::cout << test_path.size() << std::endl;
	for(char c : test_path){
		transition(c);
	}
	end_of_stream();

	for(const Match& m : matches){
		std::cout << m << std::endl;
	}

	return 0;
}