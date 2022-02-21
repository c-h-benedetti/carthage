#include "StateMachine.hpp"

std::ostream& operator<<(std::ostream& o, const Match& m){
	o << "(" << m.p_start << " -> " << m.p_end << ")";
	return o;
}
