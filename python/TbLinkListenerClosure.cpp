/*
 * TbLinkListenerClosure.cpp
 *
 *  Created on: Jan 18, 2022
 *      Author: mballance
 */

#include "TbLinkListenerClosure.h"
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/ITbLinkEvent.h"
#include "native.h"

using namespace tblink_rpc_core;

TbLinkListenerClosure::TbLinkListenerClosure(PyObject *hndl) : m_hndl(hndl) {
	// TODO Auto-generated constructor stub

}

TbLinkListenerClosure::~TbLinkListenerClosure() {
	// TODO Auto-generated destructor stub
}

void TbLinkListenerClosure::event(const ITbLinkEvent *ev) {
	tblink_listener_event(m_hndl, const_cast<ITbLinkEvent *>(ev));
}

