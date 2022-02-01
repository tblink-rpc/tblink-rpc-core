/*
 * InterfaceImplClosure.cpp
 *
 *  Created on: Jan 31, 2022
 *      Author: mballance
 */

#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/ITbLinkEvent.h"
#include "native.h"
#include "InterfaceImplClosure.h"

InterfaceImplClosure::InterfaceImplClosure(PyObject *req_f) : m_req_f(req_f) {
	 Py_INCREF(m_req_f);

}

InterfaceImplClosure::~InterfaceImplClosure() {
	Py_DECREF(m_req_f);
}

void InterfaceImplClosure::invoke(
			tblink_rpc_core::IInterfaceInst	*ifinst,
			tblink_rpc_core::IMethodType	*method,
			intptr_t						call_id,
			tblink_rpc_core::IParamValVec	*params) {
	interface_inst_req_f(m_req_f, ifinst, method, call_id, params);
}

