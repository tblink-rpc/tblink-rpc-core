/*
 * invoke_req_closure.cpp
 *
 *  Created on: Dec 1, 2021
 *      Author: mballance
 */

#include <Python.h>
#include <functional>
#include "tblink_rpc/IInterfaceInst.h"
#include "native.h"

using namespace tblink_rpc_core;

extern "C" invoke_req_f invoke_req_closure(PyObject *obj) {
	return std::bind(&interface_inst_req_f, obj,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4);
}



