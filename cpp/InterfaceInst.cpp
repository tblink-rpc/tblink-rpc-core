/*
 * JsonInterfaceInst.cpp
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#include "InterfaceInst.h"

#include "EndpointMsgTransport.h"

namespace tblink_rpc_core {

InterfaceInst::InterfaceInst(
		EndpointMsgTransport			*endpoint,
		IInterfaceType					*type,
		const std::string				&inst_name,
		bool							is_mirror) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_is_mirror = is_mirror;
	m_call_id = 0;
}

InterfaceInst::InterfaceInst(
		EndpointMsgTransport			*endpoint,
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

InterfaceInst::~InterfaceInst() {
	// TODO Auto-generated destructor stub
}

/**
 * Called in response to an invoke request from the peer endpoint
 */
void InterfaceInst::invoke_req(
			IMethodType				*method,
			IParamValVec			*params,
			const invoke_rsp_f		&response_f) {
	intptr_t call_id = m_call_id;
	m_call_id += 1;
	m_invoke_m.insert({call_id, response_f});
	m_req_f(this, method, call_id, params);
}

IParamVal *InterfaceInst::invoke(
		IMethodType									*method,
		IParamValVec								*params) {
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
		intptr_t id = m_endpoint->send_req(
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

		intptr_t id = m_endpoint->send_req(
				"tblink.invoke-nb",
				r_params,
				std::bind(&InterfaceInst::invoke_nb_rsp, this,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3));

		while (!have_rsp) {
			if (m_endpoint->process_one_message() == -1) {
				break;
			}
		}
	}

	return ret;
}

int32_t InterfaceInst::invoke_nb(
		IMethodType									*method,
		IParamValVec								*params,
		const invoke_rsp_f							&completion_f) {
	IParamValMap *r_params = m_endpoint->mkValMap();
	intptr_t call_id = m_call_id;
	m_call_id += 1;

	// Register the completion function
	m_invoke_m.insert({call_id, completion_f});

	r_params->setVal("ifinst", m_endpoint->mkValStr(name()));
	r_params->setVal("method", m_endpoint->mkValStr(method->name()));
	r_params->setVal("call-id", m_endpoint->mkValIntU(call_id, 64));
	r_params->setVal("params", params);

	if (method->is_blocking()) {
		intptr_t id = m_endpoint->send_req(
				"tblink.invoke-nb",
				r_params);
	} else {
		intptr_t id = m_endpoint->send_req(
				"tblink.invoke-nb",
				r_params);
	}

	return 0;
}

/**
 * Called to complete a local call.
 */
void InterfaceInst::invoke_rsp(
		intptr_t									call_id,
		IParamVal									*ret) {
	std::unordered_map<intptr_t,invoke_rsp_f>::const_iterator it;

	if ((it=m_invoke_m.find(call_id)) != m_invoke_m.end()) {
		it->second(ret);
		m_invoke_m.erase(it);
	} else {
		fprintf(stdout, "Error: unknown call-id %lld\n", call_id);
		fflush(stdout);
	}
}

/**
 * Called when the response for a non-blocking invoke is received
 */
void InterfaceInst::invoke_nb_rsp(
		intptr_t									id,
		IParamValMap								*result,
		IParamValMap								*error) {
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
}

/**
 * Called in response to a response-request message from the
 * remote peer endpoint
 */
void InterfaceInst::invoke_b_rsp(
			intptr_t									call_id,
			IParamVal									*ret) {
	std::unordered_map<intptr_t,invoke_rsp_f>::const_iterator it;
	if ((it=m_outbound_invoke_m.find(call_id)) != m_outbound_invoke_m.end()) {
		fprintf(stdout, "--> Receive response\n");
		fflush(stdout);
		it->second(ret);
		m_outbound_invoke_m.erase(it);
		fprintf(stdout, "<-- Receive response\n");
		fflush(stdout);
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
}

IParamValBool *InterfaceInst::mkValBool(bool val) {
	return m_endpoint->mkValBool(val);
}

IParamValInt *InterfaceInst::mkValIntU(uint64_t val, int32_t width) {
	return m_endpoint->mkValIntU(val, width);
}

IParamValInt *InterfaceInst::mkValIntS(int64_t val, int32_t width) {
	return m_endpoint->mkValIntS(val, width);
}

IParamValMap *InterfaceInst::mkValMap() {
	return m_endpoint->mkValMap();
}

IParamValStr *InterfaceInst::mkValStr(const std::string &val) {
	return m_endpoint->mkValStr(val);
}

IParamValVec *InterfaceInst::mkValVec() {
	return m_endpoint->mkValVec();
}

} /* namespace tblink_rpc_core */
