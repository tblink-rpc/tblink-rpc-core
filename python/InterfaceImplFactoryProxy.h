/*
 * InterfaceImplFactoryProxy.h
 *
 *  Created on: Feb 6, 2022
 *      Author: mballance
 */

#pragma once
#include <Python.h>
#include "tblink_rpc/IInterfaceImplFactory.h"

class InterfaceImplFactoryProxy  : public tblink_rpc_core::IInterfaceImplFactory {
public:
	InterfaceImplFactoryProxy(PyObject *factory);

	virtual ~InterfaceImplFactoryProxy();

	virtual tblink_rpc_core::IInterfaceImpl *createImpl() override;

private:
	PyObject				*m_factory;

};

