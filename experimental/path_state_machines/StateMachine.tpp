template <typename T>
void StateMachine<T>::reset(){
	this->matches.clear();
	this->current = 0;
	this->position = 0;
	this->match.reset();
}

template <typename T>
void StateMachine<T>::init(){
	this->reset();
}
