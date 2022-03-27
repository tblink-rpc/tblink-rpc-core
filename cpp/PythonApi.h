/*
 * PythonApi.h
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#pragma once
#include <stdint.h>
#include <string>

namespace tblink_rpc_core {

class PythonApi {
public:
	PythonApi();

	virtual ~PythonApi();

	int32_t init(const std::string &pylib);

	void Py_Initialize() { Py_Initialize_f(); }

	void Py_EndInterpreter(void *interp) { Py_EndInterpreter(interp); }

	void *Py_NewInterpreter() { return Py_NewInterpreter(); }

	void PyEval_AcquireLock() { PyEval_AcquireLock_f(); }

	void PyEval_AcquireThread(void *t) { PyEval_AcquireThread_f(t); }

	void PyEval_ReleaseThread(void *t) { PyEval_ReleaseThread_f(t); }

	void PyEval_ReleaseLock() { PyEval_ReleaseLock_f(); }

	void PyErr_Print() { PyErr_Print_f(); }

	void *PyImport_ImportModule(const std::string &name) {
		return PyImport_ImportModule_f(name.c_str());
	}

	void *PyList_New() {
		return PyList_New_f(0);
	}

	void *PyObject_GetAttrString(void *m, const std::string &name) {
		return PyObject_GetAttrString_f(m, name.c_str());
	}

	void *PyObject_CallObject(void *c, void *a) {
		return PyObject_CallObject_f(c, a);
	}

	void *PyTuple_New(uint32_t sz) {
		return PyTuple_New_f(sz);
	}

	int PyTuple_SetItem(void *t, uint32_t i, void *o) {
		return PyTuple_SetItem_f(t, i, o);
	}

	void *PyUnicode_FromString(const std::string &s) {
		return PyUnicode_FromString_f(s.c_str());
	}

private:
	void (*Py_Initialize_f)();
	void (*Py_EndInterpreter_f)(void *);
	void *(*Py_NewInterpreter_f)();
	void (*PyEval_AcquireLock_f)();
	void (*PyEval_AcquireThread_f)(void *);
	void (*PyEval_ReleaseThread_f)(void *);
	void (*PyEval_ReleaseLock_f)();
	void (*PyErr_Print_f)();
	void *(*PyImport_ImportModule_f)(const char *name);
	void *(*PyList_New_f)(int);
	int (*PyList_Append_f)(void *, void*);
	void *(*PyObject_CallObject_f)(void *, void *);
	void *(*PyObject_GetAttrString_f)(void *, const char *);
	void *(*PyTuple_New_f)(uint32_t);
	int (*PyTuple_SetItem_f)(void *, uint32_t, void *);
	void *(*PyUnicode_FromString_f)(const char *);
};

} /* namespace tblink_rpc_core */

