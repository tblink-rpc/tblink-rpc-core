/*
 * InterfaceImplClosure.h
 *
 *  Created on: Jan 31, 2022
 *      Author: mballance
 */

#pragma once
#include <Python.h>
#include "tblink_rpc/IInterfaceImpl.h"

class InterfaceImplClosure : public tblink_rpc_core::IInterfaceImpl {
public:

	InterfaceImplClosure(PyObject *req_f);

	virtual ~InterfaceImplClosure();

	virtual void init(tblink_rpc_core::IInterfaceInst *ifinst) override;

	virtual void invoke(
			tblink_rpc_core::IInterfaceInst	*ifinst,
			tblink_rpc_core::IMethodType	*method,
			intptr_t						call_id,
			tblink_rpc_core::IParamValVec	*params) override;

private:
	PyObject						*m_req_f;

};

