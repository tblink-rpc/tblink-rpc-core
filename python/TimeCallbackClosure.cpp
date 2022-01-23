/*
 * TimeCallbackClosure.cpp
 *
 *  Created on: Jan 22, 2022
 *      Author: mballance
 */

#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IInterfaceInst.h"
#include "tblink_rpc/ITbLinkEvent.h"
#include "native.h"
#include "TimeCallbackClosure.h"

using namespace tblink_rpc_core;

TimeCallbackClosure::TimeCallbackClosure(PyObject *obj) : m_obj(obj) {
	Py_INCREF(m_obj);
}

TimeCallbackClosure::~TimeCallbackClosure() {
	// TODO Auto-generated destructor stub
}

std::function<void()> TimeCallbackClosure::closure() {
	return std::bind(&TimeCallbackClosure::callback, this);
}

void TimeCallbackClosure::callback() {
	time_callback_f(m_obj);
	Py_DECREF(m_obj);
	delete this;
}

extern "C" time_cb_f time_callback_closure(PyObject *obj) {
	TimeCallbackClosure *c = new TimeCallbackClosure(obj);
	return c->closure();
}

