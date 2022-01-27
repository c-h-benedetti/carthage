#ifndef PY_C_OBJECT_BASIS_HEADER_INCLUDED
#define PY_C_OBJECT_BASIS_HEADER_INCLUDED

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <memory>
#include <functional>
#include <iostream>

class PyCObject {

protected:

	PyObject* object = nullptr;

public:

	// CONSTRUCTORS

	PyCObject() = delete;
	PyCObject(PyObject*& o) = delete;
	PyCObject(PyObject*&& o);
	PyCObject(const PyCObject& o);
	PyCObject(PyCObject&& o);

	PyCObject& operator=(PyObject*& obj) = delete;
	PyCObject& operator=(PyObject*&& obj);
	PyCObject& operator=(const PyCObject& o);
	PyCObject& operator=(PyCObject&& o);

	// DESTRUCTOR

	~PyCObject();

	// ACCESSORS



	// METHODS

	void nullify();

	// OPERATORS

	inline bool operator==(const PyCObject& o) const{
		return this->object == o.object;
	}
	
	inline operator bool() const{
		return this->object;
	}

};


#endif // PY_C_OBJECT_BASIS_HEADER_INCLUDED

