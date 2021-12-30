/*
 * PythonApi.cpp
 *
 *  Created on: Dec 29, 2021
 *      Author: mballance
 */

#include "PythonApi.h"
#include "tblink_rpc/ISymFinder.h"
#include "TbLink.h"

namespace tblink_rpc_core {

PythonApi::PythonApi() {
	Py_Initialize_f = 0;
	PyImport_ImportModule_f = 0;

}

PythonApi::~PythonApi() {
	// TODO Auto-generated destructor stub
}

int32_t PythonApi::init(const std::string &pylib) {
	ITbLink *tblink = TbLink::inst();
	ISymFinder::result_t lib_r = tblink->load_library(pylib);
	ISymFinder *lib = lib_r.first.get();

	Py_Initialize_f = lib->findSymT<void (*)()>("Py_Initialize");
	if (!Py_Initialize_f) {
		return -1;
	}

	PyImport_ImportModule_f = lib->findSymT<void *(*)(const char *)>("PyImport_ImportModule");
	if (!PyImport_ImportModule_f) {
		return -1;
	}

	return 0;
}

} /* namespace tblink_rpc_core */
