/*
 * JsonInterfaceInst.cpp
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#include "Debug.h"
#include "EndpointMsgBase.h"
#include "InterfaceInstBase.h"

#define DEBUG_INTERFACE_INST_BASE

#ifdef DEBUG_INTERFACE_INST_BASE
#define DEBUG_ENTER(fmt, ...) \
	DEBUG_ENTER_BASE(InterfaceInstBase, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) \
	DEBUG_LEAVE_BASE(InterfaceInstBase, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
	DEBUG_BASE(InterfaceInstBase, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

InterfaceInstBase::InterfaceInstBase(
		IEndpoint						*endpoint,
		IInterfaceType					*type,
		const std::string				&inst_name,
		bool							is_mirror) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_is_mirror = is_mirror;
	m_call_id = 0;
}

InterfaceInstBase::InterfaceInstBase(
		IEndpoint						*endpoint,
		IInterfaceType					*type,
		const std::string				&inst_name,
		bool							is_mirror,
		const invoke_req_f				&req_f) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_is_mirror = is_mirror;
	m_req_f = req_f;
	m_call_id = 0;
}

InterfaceInstBase::~InterfaceInstBase() {
	// TODO Auto-generated destructor stub
}

/**
 * Called in response to an invoke request from the peer endpoint
 */
void InterfaceInstBase::invoke_req(
			IMethodType				*method,
			IParamValVec			*params,
			const invoke_rsp_f		&response_f) {
	DEBUG_ENTER("invoke_req");
	intptr_t call_id = m_call_id;
	m_call_id += 1;
	m_invoke_m.insert({call_id, response_f});
	m_req_f(this, method, call_id, params);
	DEBUG_LEAVE("invoke_req");
}

/**
 * Called to complete a local call.
 */
void InterfaceInstBase::invoke_rsp(
		intptr_t									call_id,
		IParamVal									*ret) {
	DEBUG_ENTER("invoke_rsp");
	std::unordered_map<intptr_t,invoke_rsp_f>::const_iterator it;

	if ((it=m_invoke_m.find(call_id)) != m_invoke_m.end()) {
		it->second(ret);
		m_invoke_m.erase(it);
	} else {
		fprintf(stdout, "Error: unknown call-id %lld\n", call_id);
		fflush(stdout);
	}
	DEBUG_LEAVE("invoke_rsp");
}

/**
 * Called when the response for a non-blocking invoke is received
 */
void InterfaceInstBase::invoke_nb_rsp(
		intptr_t									id,
		IParamValMap								*result,
		IParamValMap								*error) {
	DEBUG_ENTER("invoke_nb_rsp");
	int64_t call_id = result->getValT<IParamValInt>("call-id")->val_s();
	std::unordered_map<intptr_t,invoke_rsp_f>::const_iterator it;

	if ((it=m_outbound_invoke_m.find(call_id)) != m_outbound_invoke_m.end()) {
		IParamVal *retval = 0;

		if (result->hasKey("return")) {
			retval = result->getVal("return");
		}

		it->second(retval);

		m_outbound_invoke_m.erase(it);
	} else {
		fprintf(stdout, "Error: unknown call-id %lld\n", call_id);
		fflush(stdout);
	}
	DEBUG_LEAVE("invoke_nb_rsp");
}

/**
 * Called in response to a response-request message from the
 * remote peer endpoint
 */
void InterfaceInstBase::invoke_b_rsp(
			intptr_t									call_id,
			IParamVal									*ret) {
	DEBUG_ENTER("invoke_b_rsp");
	std::unordered_map<intptr_t,invoke_rsp_f>::const_iterator it;
	if ((it=m_outbound_invoke_m.find(call_id)) != m_outbound_invoke_m.end()) {
		DEBUG("--> Receive response\n");
		it->second(ret);
		m_outbound_invoke_m.erase(it);
		DEBUG("<-- Receive response\n");
	} else {
		fprintf(stdout, "Error: unknown call-response id %lld\n", call_id);
		fflush(stdout);
	}
#ifdef UNDEFINED
	if ((it=m_invoke_m.find(call_id)) != m_invoke_m.end()) {
		fprintf(stdout, "--> Send response\n");
		fflush(stdout);
		it->second(ret);
		fprintf(stdout, "<-- Send response\n");
		fflush(stdout);
		m_invoke_m.erase(it);
	} else {
		fprintf(stdout, "Error: unknown call-id %lld\n", call_id);
		fflush(stdout);
	}
#endif
	DEBUG_LEAVE("invoke_b_rsp");
}

IParamValBool *InterfaceInstBase::mkValBool(bool val) {
	return m_endpoint->mkValBool(val);
}

IParamValInt *InterfaceInstBase::mkValIntU(uint64_t val, int32_t width) {
	return m_endpoint->mkValIntU(val, width);
}

IParamValInt *InterfaceInstBase::mkValIntS(int64_t val, int32_t width) {
	return m_endpoint->mkValIntS(val, width);
}

IParamValMap *InterfaceInstBase::mkValMap() {
	return m_endpoint->mkValMap();
}

IParamValStr *InterfaceInstBase::mkValStr(const std::string &val) {
	return m_endpoint->mkValStr(val);
}

IParamValVec *InterfaceInstBase::mkValVec() {
	return m_endpoint->mkValVec();
}

} /* namespace tblink_rpc_core */
