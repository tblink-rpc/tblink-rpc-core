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

	struct fm_t {
		void **fp_p;
		const char *fn;
	};

	fm_t func_t[] = {
			{(void **)&Py_Initialize_f, "Py_Initialize"},
			{(void **)&PyErr_Print_f, "PyErr_Print"},
			{(void **)&PyImport_ImportModule_f, "PyImport_ImportModule"},
			{(void **)&PyList_New_f, "PyList_New"},
			{(void **)&PyObject_CallObject_f, "PyObject_CallObject"},
			{(void **)&PyObject_GetAttrString_f, "PyObject_GetAttrString"},
			{(void **)&PyTuple_New_f, "PyTuple_New"},
			{(void **)&PyTuple_SetItem_f, "PyTuple_SetItem"},
			{(void **)&PyUnicode_FromString_f, "PyUnicode_FromString"},
	};

	for (uint32_t i=0; i<sizeof(func_t)/sizeof(fm_t); i++) {
		*(func_t[i].fp_p) = lib->findSym(func_t[i].fn);

		if (!*func_t[i].fp_p) {
			fprintf(stdout, "Error: Failed to find %s\n", func_t[i].fn);
			return -1;
		}
	}

	return 0;
}

} /* namespace tblink_rpc_core */
