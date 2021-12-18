/*
 * InterfaceInstMsg.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#include "Debug.h"
#include "EndpointMsgTransport.h"
#include "InterfaceInstMsgTransport.h"

#define DEBUG_INTERFACE_INST_MSG_TRANSPORT

#ifdef DEBUG_INTERFACE_INST_MSG_TRANSPORT
#define DEBUG_ENTER(fmt, ...) \
	DEBUG_ENTER_BASE(InterfaceInstMsgTransport, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) \
	DEBUG_LEAVE_BASE(InterfaceInstMsgTransport, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
	DEBUG_BASE(InterfaceInstMsgTransport, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

InterfaceInstMsgTransport::InterfaceInstMsgTransport(
		EndpointMsgBase				*endpoint,
		IInterfaceType				*type,
		const std::string			&inst_name,
		bool						is_mirror) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror) {
}

InterfaceInstMsgTransport::InterfaceInstMsgTransport(
		EndpointMsgBase				*endpoint,
		IInterfaceType				*type,
		const std::string			&inst_name,
		bool						is_mirror,
		const invoke_req_f			&req_f) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror, req_f) {

}

InterfaceInstMsgTransport::~InterfaceInstMsgTransport() {
	// TODO Auto-generated destructor stub
}

IParamVal *InterfaceInstMsgTransport::invoke(
		IMethodType									*method,
		IParamValVec								*params) {
	DEBUG_ENTER("invoke");
	IParamValMap *r_params = m_endpoint->mkValMap();
	intptr_t call_id = m_call_id;
	m_call_id += 1;

	r_params->setVal("ifinst", m_endpoint->mkValStr(name()));
	r_params->setVal("method", m_endpoint->mkValStr(method->name()));
	r_params->setVal("call-id", m_endpoint->mkValIntU(call_id, 64));
	r_params->setVal("params", params);

	IParamVal *ret = 0;
	bool have_rsp = false;

	m_outbound_invoke_m.insert({call_id, [&](IParamVal *retval) {
		have_rsp = true;
		if (retval) {
			ret = retval->clone();
		}
	}});

	if (method->is_blocking()) {
		intptr_t id = dynamic_cast<EndpointMsgBase *>(m_endpoint)->send_req(
				"tblink.invoke-b",
				r_params);

		// TODO: wait for a response via the m_invoke_m

		// TODO: wait for a response via a matching request
		while (!have_rsp) {
			if (m_endpoint->run_until_event() == -1) {
				break;
			}
		}
	} else {
		bool recv_rsp = false;

		intptr_t id = dynamic_cast<EndpointMsgBase *>(m_endpoint)->send_req(
				"tblink.invoke-nb",
				r_params,
				std::bind(&InterfaceInstBase::invoke_nb_rsp, this,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3));

		while (!have_rsp) {
			if (m_endpoint->process_one_message() == -1) {
				break;
			}
		}
	}

	DEBUG_LEAVE("invoke");

	return ret;
}

int32_t InterfaceInstMsgTransport::invoke_nb(
		IMethodType									*method,
		IParamValVec								*params,
		const invoke_rsp_f							&completion_f) {
	int32_t ret = 0;
	DEBUG_ENTER("invoke_nb");
	IParamValMap *r_params = m_endpoint->mkValMap();
	intptr_t call_id = m_call_id;
	m_call_id += 1;

	ret = dynamic_cast<EndpointMsgBase *>(m_endpoint)->invoke_nb(
			this,
			method,
			params,
			completion_f);

	DEBUG_LEAVE("invoke_nb");
	return ret;
#ifdef UNDEFINED
	// Register the completion function
//	m_invoke_m.insert({call_id, completion_f});

	if (method->is_blocking()) {
		r_params->setVal("ifinst", m_endpoint->mkValStr(name()));
		r_params->setVal("method", m_endpoint->mkValStr(method->name()));
		r_params->setVal("call-id", m_endpoint->mkValIntU(call_id, 64));
		r_params->setVal("params", params);

		intptr_t id = dynamic_cast<EndpointMsgBase *>(m_endpoint)->send_req(
				"tblink.invoke-b",
				r_params);
	}


	m_outbound_invoke_m.insert({call_id, completion_f});
	DEBUG("Insert call-id %lld in outbound_invoke_m", call_id);

	if (method->is_blocking()) {
	} else {
		intptr_t id = dynamic_cast<EndpointMsgBase *>(m_endpoint)->send_req(
				"tblink.invoke-nb",
				r_params,
				std::bind(&InterfaceInstBase::invoke_nb_rsp, this,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3));
	}

	DEBUG_LEAVE("invoke_nb");
	return 0;
#endif
}

} /* namespace tblink_rpc_core */
