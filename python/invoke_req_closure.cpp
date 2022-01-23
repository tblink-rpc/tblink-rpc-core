/*
 * invoke_req_closure.cpp
 *
 *  Created on: Dec 1, 2021
 *      Author: mballance
 */

#include <Python.h>
#include <functional>
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/ITbLinkEvent.h"
#include "native.h"

using namespace tblink_rpc_core;

extern "C" invoke_req_f invoke_req_closure(PyObject *obj) {
//	PyInit_native();
	return std::bind(&interface_inst_req_f, obj,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4);
}

extern "C" invoke_rsp_f invoke_rsp_closure(PyObject *obj) {
//	PyInit_native();
	return std::bind(&interface_inst_rsp_f, obj,
			std::placeholders::_1);
}

extern "C" endpoint_ev_f endpoint_ev_closure(PyObject *obj) {
//	PyInit_native();
//        Py_XINCREF(obj);
	return std::bind(
			&endpoint_ev_callback_f,
			obj,
			std::placeholders::_1);
}



