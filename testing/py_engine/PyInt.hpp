#ifndef PY_INT_HEADER_INCLUDED
#define PY_INT_HEADER_INCLUDED

#include "PyCObject.hpp"

class PyInt : public PyCObject{

public:

	PyInt();
	PyInt(long int i);

	PyInt& operator=(const long int& i);
	PyInt& operator=(long int&& i);

	inline operator long int() const{
		return PyLong_AsLong(this->object);
	}

};

std::ostream& operator<<(std::ostream& o, const PyInt& i);

#endif // PY_INT_HEADER_INCLUDED