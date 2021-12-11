/*
 * InterfaceInstLoopback.cpp
 *
 *  Created on: Nov 29, 2021
 *      Author: mballance
 */

#include "EndpointLoopback.h"
#include "InterfaceInstLoopback.h"

namespace tblink_rpc_core {

InterfaceInstLoopback::InterfaceInstLoopback(
		EndpointLoopback		*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name,
		bool					is_mirror) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror),
				m_peer(0), m_call_id(1) {

}

InterfaceInstLoopback::InterfaceInstLoopback(
		EndpointLoopback		*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name,
		bool					is_mirror,
		const invoke_req_f		&req_f) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror, req_f),
				m_peer(0), m_call_id(1) {

}

InterfaceInstLoopback::~InterfaceInstLoopback() {
	// TODO Auto-generated destructor stub
}

void InterfaceInstLoopback::peer(InterfaceInstLoopback *p) {
	m_peer = p;

	// Create map of method/method
	IInterfaceType *p_t = p->type();
	IInterfaceType *this_t = type();

	for (auto m_it=p_t->methods().begin();
			m_it!=p_t->methods().end(); m_it++) {
		IMethodType *this_m = this_t->findMethod((*m_it)->name());
		m_method_m.insert({*m_it, this_m});
	}
}

IParamVal *InterfaceInstLoopback::invoke(
		IMethodType									*method,
		IParamValVec								*params) {
	return 0;
}

int32_t InterfaceInstLoopback::invoke_nb(
		IMethodType									*method,
		IParamValVec								*params,
		const invoke_rsp_f							&completion_f) {
	intptr_t call_id = m_call_id;
	m_call_id += 1;

	m_rsp_m.insert({call_id, completion_f});

	return m_peer->req_invoke_nb(
			method,
			call_id,
			params);
}

int32_t InterfaceInstLoopback::req_invoke_nb(
		IMethodType									*method,
		intptr_t									call_id,
		IParamValVec								*params) {
	auto it = m_method_m.find(method);

	if (it != m_method_m.end()) {
		m_req_f(
				this,
				it->second,
				call_id,
				params);
		return 0;
	} else {
		return -1;
	}
}

void InterfaceInstLoopback::rsp_invoke(
		intptr_t									call_id,
		IParamVal									*retval) {
	auto it = m_rsp_m.find(call_id);

	if (it != m_rsp_m.end()) {
		it->second(retval);
		m_rsp_m.erase(it);
	}
}

} /* namespace tblink_rpc_core */
