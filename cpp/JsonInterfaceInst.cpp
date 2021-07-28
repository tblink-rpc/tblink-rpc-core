/*
 * JsonInterfaceInst.cpp
 *
 *  Created on: Jul 13, 2021
 *      Author: mballance
 */

#include "JsonInterfaceInst.h"
#include "JsonRpcEndpoint.h"

namespace tblink_rpc_core {

JsonInterfaceInst::JsonInterfaceInst(
		JsonRpcEndpoint			*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_call_id = 0;
}

JsonInterfaceInst::JsonInterfaceInst(
		JsonRpcEndpoint			*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name,
		const invoke_req_f		&req_f) {
	m_endpoint = endpoint;
	m_type = type;
	m_inst_name = inst_name;
	m_req_f = req_f;
	m_call_id = 0;
}

JsonInterfaceInst::~JsonInterfaceInst() {
	// TODO Auto-generated destructor stub
}

void JsonInterfaceInst::invoke_req(
			IMethodType				*method,
			IParamValVectorSP		params,
			const invoke_rsp_f		&response_f) {
	intptr_t call_id = m_call_id;
	m_call_id += 1;
	m_invoke_m.insert({call_id, response_f});
	m_req_f(this, method, call_id, params);
}

int32_t JsonInterfaceInst::invoke(
		IMethodType									*method,
		IParamValVectorSP							params,
		const invoke_rsp_f							&completion_f) {

	// TODO:
	IParamValMapSP r_params = m_endpoint->mkValMap();
	r_params->setVal("ifinst", m_endpoint->mkValStr(name()));
	r_params->setVal("method", m_endpoint->mkValStr(method->name()));
	r_params->setVal("params", params);
	intptr_t id = m_endpoint->send_req(
			"tblink.invoke",
			r_params);

	std::pair<IParamValMapSP,IParamValMapSP> rsp = m_endpoint->wait_rsp(id);

	return 0;
}

IParamValSP JsonInterfaceInst::invoke_nb(
		IMethodType									*method,
		IParamValVectorSP							params) {

	IParamValMapSP r_params = m_endpoint->mkValMap();
	r_params->setVal("ifinst", m_endpoint->mkValStr(name()));
	r_params->setVal("method", m_endpoint->mkValStr(method->name()));
	r_params->setVal("params", params);
	intptr_t id = m_endpoint->send_req(
			"tblink.invoke-nb",
			r_params);

	std::pair<IParamValMapSP,IParamValMapSP> rsp = m_endpoint->wait_rsp(id);

	if (rsp.first->hasKey("return")) {
		return rsp.first->getVal("return");
	} else {
		return 0;
	}
}

void JsonInterfaceInst::invoke_rsp(
		intptr_t									call_id,
		IParamValSP									ret) {
	std::map<intptr_t,invoke_rsp_f>::const_iterator it;

	if ((it=m_invoke_m.find(call_id)) != m_invoke_m.end()) {
		it->second(ret);
		m_invoke_m.erase(it);
	}
}

IParamValBoolSP JsonInterfaceInst::mkValBool(bool val) {
	return m_endpoint->mkValBool(val);
}

IParamValIntSP JsonInterfaceInst::mkValIntU(uint64_t val) {
	return m_endpoint->mkValIntU(val);
}

IParamValIntSP JsonInterfaceInst::mkValIntS(int64_t val) {
	return m_endpoint->mkValIntS(val);
}

IParamValMapSP JsonInterfaceInst::mkValMap() {
	return m_endpoint->mkValMap();
}

IParamValStrSP JsonInterfaceInst::mkValStr(const std::string &val) {
	return m_endpoint->mkValStr(val);
}

IParamValVectorSP JsonInterfaceInst::mkVector() {
	return m_endpoint->mkVector();
}

} /* namespace tblink_rpc_core */
