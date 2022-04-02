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

	bool init(const std::string &pylib);

	void Py_Initialize() const { Py_Initialize_f(); }

	void *PyGILState_Ensure() const { return PyGILState_Ensure_f(); }

	void PyGILState_Release(void *s) const { PyGILState_Release_f(s); }

	void Py_EndInterpreter(void *interp) const { Py_EndInterpreter(interp); }

	void *Py_NewInterpreter() const { return Py_NewInterpreter_f(); }

	void PyEval_InitThreads() const { PyEval_InitThreads_f(); }

	void PyEval_AcquireLock() const { PyEval_AcquireLock_f(); }

	void PyEval_AcquireThread(void *t) const { PyEval_AcquireThread_f(t); }

	void PyEval_ReleaseThread(void *t) const { PyEval_ReleaseThread_f(t); }

	void PyEval_ReleaseLock() const { PyEval_ReleaseLock_f(); }

	void PyEval_RestoreThread(void *t) const { PyEval_RestoreThread_f(t); }

	void *PyEval_SaveThread() const { return PyEval_SaveThread_f(); }

	void PyErr_Print() const { PyErr_Print_f(); }

	void *PyImport_ImportModule(const std::string &name) const {
		return PyImport_ImportModule_f(name.c_str());
	}

	void *PyList_New() const {
		return PyList_New_f(0);
	}

	void *PyObject_GetAttrString(void *m, const std::string &name) const {
		return PyObject_GetAttrString_f(m, name.c_str());
	}

	void *PyObject_CallObject(void *c, void *a) const {
		return PyObject_CallObject_f(c, a);
	}

	void *PyTuple_New(uint32_t sz) const{
		return PyTuple_New_f(sz);
	}

	int PyTuple_SetItem(void *t, uint32_t i, void *o) const {
		return PyTuple_SetItem_f(t, i, o);
	}

	void *PyUnicode_FromString(const std::string &s) const {
		return PyUnicode_FromString_f(s.c_str());
	}

private:
	void (*Py_Initialize_f)();
	void *(*PyGILState_Ensure_f)();
	void (*PyGILState_Release_f)(void *s);
	void (*Py_EndInterpreter_f)(void *);
	void *(*Py_NewInterpreter_f)();
	void (*PyEval_InitThreads_f)();
	void (*PyEval_AcquireLock_f)();
	void (*PyEval_AcquireThread_f)(void *);
	void (*PyEval_ReleaseThread_f)(void *);
	void (*PyEval_ReleaseLock_f)();
	void (*PyEval_RestoreThread_f)(void *);
	void *(*PyEval_SaveThread_f)();
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

