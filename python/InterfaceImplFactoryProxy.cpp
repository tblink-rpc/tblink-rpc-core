/*
 * InterfaceImplFactoryProxy.cpp
 *
 *  Created on: Feb 6, 2022
 *      Author: mballance
 */

#include "InterfaceImplFactoryProxy.h"

InterfaceImplFactoryProxy::InterfaceImplFactoryProxy(
		PyObject *factory) : m_factory(factory) {
	Py_INCREF(m_factory);
}

InterfaceImplFactoryProxy::~InterfaceImplFactoryProxy() {
	// TODO Auto-generated destructor stub
	Py_DECREF(m_factory);
}

tblink_rpc_core::IInterfaceImpl *InterfaceImplFactoryProxy::createImpl() {

}

