#include "MatchDirectories.hpp"

inline void MatchDirectories::set_state(State s){
	switch(s){
		case (State::START):
			if (this->current == State::STATE1){
				this->matches.push_back(this->match);
				this->save();
			}
			if (this->current == State::MATCH){
				this->matches.push_back(this->match);
				this->save();
			}
			this->current = State::START;
			break;

		case (State::STATE1):
			this->current = State::STATE1;
			break;

		case (State::MATCH):
			if (this->current == State::START){
				this->match.p_start = this->position;
				this->match.p_end = this->position + 1;
			}
			else if (this->current == State::MATCH){
				this->matches.push_back(this->match);
				this->save();
				this->match.p_start = this->position;
				this->match.p_end = this->position + 1;
			}
			else if (this->current == State::STATE1){
				this->match.p_end = this->position + 1;
			}
			this->current = State::MATCH;
			break;
	};
	this->position++;
}

inline void MatchDirectories::transition(char c){
	switch(this->current){
		case(State::START):
			if (this->separator(c)){
				this->set_state(State::MATCH);
			}
			else{
				this->set_state(State::START);	
			}
			break;

		case(State::STATE1):
			if (this->valid_char(c)){
				this->set_state(State::STATE1);
			}
			else if (this->separator(c)){
				this->set_state(State::MATCH);
			}
			else{
				this->set_state(State::START);
			}
			break;

		case(State::MATCH):
			if (this->separator(c)){
				this->set_state(State::MATCH);
			}
			else if (this->valid_char(c)){
				this->set_state(State::STATE1);
			}
			else{
				this->set_state(State::START);
			}
			break;
	};
}


void MatchDirectories::end_of_stream(){
	if (current == State::MATCH){
		matches.push_back(match);
	}
	else if (current == State::STATE1){
		matches.push_back(match);
	}
}

void MatchDirectories::process(char stream[], size_t size, size_t min_size){

	this->save = [&](){
		size_t size = this->match.size();
		if (size > min_size){
			std::string buffer(size, 0);
			memcpy(&buffer[0], &stream[this->match.p_start], this->match.size());
			this->results.push_back(buffer);
		}
	};

	this->reset();
	for(size_t i = 0 ; i < size ; i++){
		this->transition(stream[i]);
	}
	this->end_of_stream();
}

void MatchDirectories::process(std::istream& stream, size_t min_size){

	this->save = [&](){
		size_t size = this->match.size();
		if (size > min_size){
			std::string buffer(size, 0);
			stream.seekg(this->match.p_start);
			stream.read(&buffer[0], size);
			stream.seekg(this->position+1);
			this->results.push_back(buffer);
		}
	};

	this->reset();
	char c;
	while(stream.read(&c, 1)){
		this->transition(c);
	}
	this->end_of_stream();
}

void MatchDirectories::process(const std::string& stream, size_t min_size){

	this->save = [&](){
		size_t size = this->match.size();
		if (size > min_size){
			this->results.push_back(stream.substr(this->match.p_start, size));
		}
	};

	this->reset();
	for(char c : stream){
		this->transition(c);
	}
	this->end_of_stream();
}
