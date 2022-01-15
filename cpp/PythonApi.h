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

	void *PyImport_ImportModule(const std::string &name) {
		return PyImport_ImportModule_f(name.c_str());
	}

private:
	void (*Py_Initialize_f)();
	void *(*PyImport_ImportModule_f)(const char *name);
};

} /* namespace tblink_rpc_core */

