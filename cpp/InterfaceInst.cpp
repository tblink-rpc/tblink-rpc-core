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
		IInterfaceType			*type,
		const std::string		&inst_name) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_call_id = 0;
}

InterfaceInst::InterfaceInst(
		EndpointMsgTransport			*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name,
		const invoke_req_f		&req_f) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_req_f = req_f;
	m_call_id = 0;
}

InterfaceInst::~InterfaceInst() {
	// TODO Auto-generated destructor stub
}

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

	/*
	// TODO:
	IParamValMap *r_params = m_endpoint->mkValMap();
	intptr_t call_id = m_call_id;
	m_call_id += 1;
	m_invoke_m.insert({call_id, completion_f});

	r_params->setVal("ifinst", m_endpoint->mkValStr(name()));
	r_params->setVal("method", m_endpoint->mkValStr(method->name()));
	r_params->setVal("call-id", m_endpoint->mkValIntU(call_id));
	r_params->setVal("params", params);
	intptr_t id = m_endpoint->send_req(
			"tblink.invoke-b",
			r_params);

	std::pair<IParamValMapSP,IParamValMapSP> rsp = m_endpoint->wait_rsp(id);
	 */

	return 0;
}

int32_t InterfaceInst::invoke_nb(
		IMethodType									*method,
		IParamValVec								*params,
		const invoke_rsp_f							&completion_f) {

	fprintf(stdout, "invoke_nb: %s\n", method->name().c_str());
	fflush(stdout);

//	return m_endpoint->invoke_nb(this, method, params);
	return 0;
}

void InterfaceInst::invoke_rsp(
		intptr_t									call_id,
		IParamVal									*ret) {
	std::map<intptr_t,invoke_rsp_f>::const_iterator it;

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
