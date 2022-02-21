#include "PyCObject.hpp"

PyCObject::PyCObject(PyObject*&& o){
	this->object = std::shared_ptr<PyObject>(o);
}


PyCObject::PyCObject(const PyCObject& o){
	this->object = o.object;
	Py_XINCREF(this->object.get());
}


PyCObject::PyCObject(PyCObject&& o){
	this->object = o.object;
	Py_XINCREF(this->object.get());
}


PyCObject& PyCObject::operator=(PyObject*&& obj){
	this->object = std::shared_ptr<PyObject>(obj);
	return *this;
}


PyCObject& PyCObject::operator=(const PyCObject& o){
	this->object = o.object;
	Py_XINCREF(this->object.get());
	return *this;
}


PyCObject& PyCObject::operator=(PyCObject&& o){
	this->object = o.object;
	Py_XINCREF(this->object.get());
	return *this;
}


PyCObject::~PyCObject(){
	this->nullify();
}


void PyCObject::nullify(){
	Py_XDECREF(this->object.get());
	this->object = nullptr;
}