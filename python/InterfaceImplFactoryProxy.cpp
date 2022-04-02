/*
 * InterfaceImplFactoryProxy.cpp
 *
 *  Created on: Feb 6, 2022
 *      Author: mballance
 */

#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/ITbLinkEvent.h"
#include "native.h"
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
	tblink_rpc_core::IInterfaceImpl *ifimpl = interface_impl_factory_proxy_createImpl(
			m_factory
			);

	return ifimpl;
}

