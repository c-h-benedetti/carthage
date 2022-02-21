#ifndef PATH_MACHINE_HPP_INCLUDED
#define PATH_MACHINE_HPP_INCLUDED

#include "StateMachine.hpp"

class PathMachine : public StateMachine<char>{
	protected:
		inline bool valid_char(char c);
		inline bool separator(char c);

	public:
		void process(const char stream[], size_t size);
		void process(std::istream& stream);
		void process(const std::string& stream);
};

inline bool PathMachine::separator(char c){
	return c == PATH_SEPARATOR;
}

inline bool PathMachine::valid_char(char c){
	return ((c >= 64) && (c <= 126)) ||
		   ((c >= 48) && (c <= 57))  ||
		   ((c >= 32) && (c <= 33))  ||
		   ((c >= 35) && (c <= 46))  ||
		    (c == 59) || (c == 61);
}

#endif //PATH_MACHINE_HPP_INCLUDED