#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*

- The C function always has two arguments, conventionally named self and args
	=> The self argument points to the module object for module-level functions; for a method it would point to the object instance.
	=> The args argument will be a pointer to a Python tuple object containing the arguments.

- PyArg_ParseTuple() returns true (nonzero) if all arguments have the right type and its components have been stored in the variables whose addresses are passed. 
  It returns false (zero) if an invalid argument list was passed.

/!\ An important convention throughout the Python interpreter is the following: when a function fails, it should set an exception condition and return an error value (usually a NULL pointer)

for void: Py_RETURN_NONE == return None (write it without the "return" in C !!!!!!)

*/

static PyObject *SpamError;

// Actual operation performed by the function
// With this name, will be called as spam.system("...") is Python

static PyObject* spam_system(PyObject* self, PyObject* args){
    const char *command;

    if (!PyArg_ParseTuple(args, "s", &command)){
        return NULL;
    }

    int sts = system(command);
    
    if (sts) {
        PyErr_SetString(SpamError, "System command failed");
        return NULL;
    }

    return PyLong_FromLong(sts);
}

// Listing of functions defined higher
// Can be devided in several tables

static PyMethodDef SpamMethods[] = {
    {
    	"system",  
    	spam_system, 
    	METH_VARARGS, // should normally always be METH_VARARGS or METH_VARARGS | METH_KEYWORDS; 0 = deprecated
		"Execute a shell command."
	},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

// Definition of a module from the table of functions

static struct PyModuleDef spammodule = {
    PyModuleDef_HEAD_INIT,
    "spam",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    SpamMethods
};

// Function initializing the module (passed to the interpreter)
// The name of this function MUST be PyInit_module_name()
// It is the only non-static element of the file
// When the Python program imports module spam for the first time, PyInit_spam() is called

PyMODINIT_FUNC PyInit_spam(void){
    PyObject *m = PyModule_Create(&spammodule);

    if (!m){
        return NULL;
    }

    SpamError = PyErr_NewException("spam.error", NULL, NULL);
    Py_XINCREF(SpamError);

    if (PyModule_AddObject(m, "error", SpamError) < 0) {
        Py_XDECREF(SpamError);
        Py_CLEAR(SpamError);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}