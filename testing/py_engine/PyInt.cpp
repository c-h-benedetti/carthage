#include "PyInt.hpp"

PyInt::PyInt() : PyCObject(PyLong_FromLong(0)){}

PyInt::PyInt(long int i) : PyCObject(PyLong_FromLong(i)){}

PyInt& PyInt::operator=(const long int& i){
	Py_XDECREF(this->object);
	this->object = PyLong_FromLong(i);
	return *this;
}

PyInt& PyInt::operator=(long int&& i){
	Py_XDECREF(this->object);
	this->object = PyLong_FromLong(i);
	return *this;
}

std::ostream& operator<<(std::ostream& o, const PyInt& i) {
	o << (long int)i;
	return o;
}