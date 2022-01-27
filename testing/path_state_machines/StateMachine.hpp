#ifndef STATE_MACHINE_HPP_INCLUDED
#define STATE_MACHINE_HPP_INCLUDED

#include <iostream>
#include <vector>

#if defined(WIN32) || defined(_WIN32)
	#define PATH_SEPARATOR '\\'
#else 
	#define PATH_SEPARATOR '/'
#endif

#define _STATE_T size_t

class Match{
	public:
		size_t p_start = 0;
		size_t p_end   = 0;

		size_t size() const{return this->p_end - this->p_start;}
		bool empty() const{return this->size() == 0;}
		void reset(){
			this->p_start = 0;
			this->p_end = 0;
		}
};

#define CONST_ITER std::vector<Match>::const_iterator

std::ostream& operator<<(std::ostream& o, const Match& m);

template <typename T>
class StateMachine{
	protected:

		Match match{};
		_STATE_T current = 0;
		std::vector<Match> matches = {};
		size_t position = 0;

	protected:

		void reset();
		void init();
		void end_of_stream();
		inline void set_state(_STATE_T s){this->current = s;}
		inline void transition(T c);

	public:

		void process(const T stream[], size_t size);
		void process(std::istream& stream);

		size_t tellg() const{return this->position;}
		size_t size() const{return this->matches.size();}

		const std::vector<Match>& get_matches() const{return this->matches;}
		CONST_ITER begin() const{return this->matches.begin();}
		CONST_ITER end() const{return this->matches.end();}
};

#include "StateMachine.tpp"

#endif //STATE_MACHINE_HPP_INCLUDED

/*

 !!! IMPORTANT NOTE !!!

 This class is meant to be used as implementation interface for a StateMachine, exclusively to avoid code duplication in children.
 The non-usage of virtual/override is made on purpose, we don't want structures such as: std::vector<StateMachine*> to be used to launch process
 However, once the process is over, nothing keeps you from storing the used instance in a dynamic-typed std::vector.
 Inlining is primordial for a state machine to not be a performances bottleneck.
 The static type of a StateMachine must be known as compile time
 As general rule: No processing method should rely on dynamic typing (transition, set_state, ...) and no processing should be launched on a dynamic typed instance.
 => A same function must not have several implementations through an inheritance branch <=> specialization forbiden
 It's allowed to declare a function in a parent without defining it, but once it's been defined, no children is allowed to specialize it.
 Methods of state machines have a pretty heavy overhead, it's a good practice to inline them.

*/