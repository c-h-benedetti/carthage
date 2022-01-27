#include <functional>
#include <cstring>
#include "PathMachine.hpp"
#include <string>

#define ITER_STR std::vector<std::string>::const_iterator

class MatchDirectories : public PathMachine{
	public:
		enum State{
			START,
			STATE1,
			MATCH
		};

	protected:

		std::vector<std::string> results = {};
		inline void set_state(State s);
		inline void transition(char c);
		void end_of_stream();

		std::function<void()> save;

	public:

		void process(char stream[], size_t size, size_t m_size=0);
		void process(std::istream& stream, size_t m_size=0);
		void process(const std::string& stream, size_t m_size=0);

		ITER_STR begin_results() const{
			return this->results.begin();
		}
		ITER_STR end_results() const{
			return this->results.end();
		}
};