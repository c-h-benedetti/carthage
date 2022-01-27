#ifndef PY_C_OBJECT_BASIS_HEADER_INCLUDED
#define PY_C_OBJECT_BASIS_HEADER_INCLUDED

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <memory>
#include <functional>

class PyCObject {

	std::shared_ptr<PyObject> object;

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

	inline PyObject* get() const{
		return this->object.get();
	}

	// METHODS

	void nullify();

	// OPERATORS

	inline bool operator==(const PyCObject& o) const{
		return this->object.get() == o.object.get();
	}
	
	inline operator bool() const{
		return this->object && static_cast<bool>(this->object.get());
	}

};

#endif // PY_C_OBJECT_BASIS_HEADER_INCLUDED


/*

[!] Implement here modifications made in the testing file

*/