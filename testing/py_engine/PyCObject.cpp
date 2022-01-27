#include "PyCObject.hpp"


PyCObject::PyCObject(PyObject*&& o){
	this->object = o;
}


PyCObject::PyCObject(const PyCObject& o){
	this->object = o.object;
	Py_XINCREF(this->object);
}


PyCObject::PyCObject(PyCObject&& o){
	this->object = o.object;
	Py_XINCREF(this->object);
}


PyCObject& PyCObject::operator=(PyObject*&& obj){
	this->object = obj;
	return *this;
}


PyCObject& PyCObject::operator=(const PyCObject& o){
	this->object = o.object;
	Py_XINCREF(this->object);
	return *this;
}


PyCObject& PyCObject::operator=(PyCObject&& o){
	this->object = o.object;
	Py_XINCREF(this->object);
	return *this;
}


PyCObject::~PyCObject(){
	this->nullify();
}


void PyCObject::nullify(){
	Py_XDECREF(this->object);
	this->object = nullptr;
}

