#include <iostream>
#include <string>
#include <vector>

enum class State;
void transition(char c);
bool separator(char c);

bool valid_char(char c){
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

void set_state(State s){
	position++;
	switch(s){
		case (State::START):
			if (match.p_start < match.p_end){
				match.p_end--;
				matches.push_back(match);
			}
			match.p_start = position;			
			current = State::START;
			break;

		case (State::STATE1):
			current = State::STATE1;
			break;

		case (State::MATCH):
			current = State::MATCH;
			match.p_end = position;
			break;
	};
}

void transition(char c){
	switch(current){
		case(State::START):
			if(separator(c)){
				set_state(State::STATE1);
			}
			else{
				set_state(State::START);
			}
			break;

		case(State::STATE1):
			if(valid_char(c)){
				set_state(State::MATCH);
			}
			else{
				set_state(State::START);
			}
			break;

		case(State::MATCH):
			if(valid_char(c)){
				set_state(State::MATCH);
			}
			else if(separator(c)){
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
}

#endif 

bool separator(char c){
	return c == PATH_SEPARATOR;
}

int main(int argc, char* argv[], char* env[]){

	std::string test_path = "ta race tu me connais pas/debut/autre/putain</sarace";
	position = 0;

	for(char c : test_path){
		transition(c);
	}
	end_of_stream();

	for(const Match& m : matches){
		std::cout << m << std::endl;
	}

	return 0;
}