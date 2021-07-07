/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include "JsonRpcEndpoint.h"

namespace tblink_rpc_core {

JsonRpcEndpoint::JsonRpcEndpoint(ITransport *t) {
	m_transport = t;

	m_transport->init(
			std::bind(
					&JsonRpcEndpoint::recv_req,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3),
			std::bind(
					&JsonRpcEndpoint::recv_rsp,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3)
					);

}

JsonRpcEndpoint::~JsonRpcEndpoint() {
	// TODO Auto-generated destructor stub
}

bool JsonRpcEndpoint::build_complete() {
	IParamValMapSP params = m_transport->mkValMap();

	intptr_t id = m_transport->send_req("tblink.build-complete", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

}

bool JsonRpcEndpoint::connect_complete() {
	IParamValMapSP params = m_transport->mkValMap();

	intptr_t id = m_transport->send_req("tblink.connect-complete", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

}

int32_t JsonRpcEndpoint::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMapSP			params) {
	fprintf(stdout, "--> recv_req: %s\n", method.c_str());
	fflush(stdout);

	fprintf(stdout, "<-- recv_req: %s\n", method.c_str());
	fflush(stdout);
	;
}

int32_t JsonRpcEndpoint::recv_rsp(
		intptr_t				id,
		IParamValSP				result,
		IParamValSP				error) {
	fprintf(stdout, "--> recv_rsp\n");
	fflush(stdout);
	m_rsp_m.insert({id, {result, error}});

	fprintf(stdout, "<-- recv_rsp\n");
	fflush(stdout);
	return 0;
}

std::pair<IParamValSP,IParamValSP> JsonRpcEndpoint::wait_rsp(intptr_t id) {
	std::map<intptr_t,std::pair<IParamValSP,IParamValSP>>::iterator it;
	std::pair<IParamValSP,IParamValSP> rsp;

	fprintf(stdout, "--> wait_rsp\n");
	fflush(stdout);

	// Poll waiting
	while ((it=m_rsp_m.find(id)) == m_rsp_m.end()) {

		fprintf(stdout, "--> poll\n");
		int32_t ret = m_transport->poll(1000);
		fprintf(stdout, "<-- poll %d\n", ret);

		if (ret == -1) {
			// Failure
			break;
		}
	}

	if (it != m_rsp_m.end()) {
		rsp = it->second;
		m_rsp_m.erase(it);
	}

	fprintf(stdout, "<-- wait_rsp\n");
	fflush(stdout);

	return rsp;
}

} /* namespace tblink_rpc_core */
