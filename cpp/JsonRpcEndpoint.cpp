/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include "JsonRpcEndpoint.h"

namespace tblink_rpc_core {

JsonRpcEndpoint::JsonRpcEndpoint(
		ITransport 				*transport,
		IEndpointServices		*services) {
	m_transport = transport;
	m_services  = services;

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

	m_build_complete = false;
	m_connect_complete = false;
	m_callback_id = 0;

}

JsonRpcEndpoint::~JsonRpcEndpoint() {
	// TODO Auto-generated destructor stub
}

bool JsonRpcEndpoint::build_complete() {
	IParamValMapSP params = m_transport->mkValMap();

	intptr_t id = m_transport->send_req("tblink.build-complete", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

	while (!m_build_complete) {
		int ret = m_transport->poll(1000);

		if (ret == -1) {
			return false;
		}
	}

	return true;
}

bool JsonRpcEndpoint::connect_complete() {
	IParamValMapSP params = m_transport->mkValMap();

	intptr_t id = m_transport->send_req("tblink.connect-complete", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

	while (!m_connect_complete) {
		int ret = m_transport->poll(1000);

		if (ret == -1) {
			return false;
		}
	}

	return true;
}

bool JsonRpcEndpoint::shutdown() {
	IParamValMapSP params = m_transport->mkValMap();

	intptr_t id = m_transport->send_req("tblink.shutdown", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

	return true;
}

intptr_t JsonRpcEndpoint::add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) {
	IParamValMapSP params = m_transport->mkValMap();
	params->setVal("time", m_transport->mkValIntU(time));
	intptr_t id = m_transport->send_req("tblink.add-time-callback", params);

	std::pair<IParamValMapSP,IParamValMapSP> rsp = wait_rsp(id);

	if (rsp.first) {
		return rsp.first->getValT<IParamValInt>("callback-id")->val_u();
	} else {
		// Error:
		return -1;
	}
}

void JsonRpcEndpoint::cancel_callback(intptr_t	callback_id) {
	IParamValMapSP params = m_transport->mkValMap();

	params->setVal("callback-id", m_transport->mkValIntU(callback_id));

	intptr_t id = m_transport->send_req("tblink.cancel-callback", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);
}

/** Called by the environment to notify that
 *  a callback has occurred
 */
void JsonRpcEndpoint::notify_callback(intptr_t   callback_id) {
	IParamValMapSP params = m_transport->mkValMap();

	params->setVal("callback-id", m_transport->mkValIntU(callback_id));

	m_transport->send_notify("tblink.notify-callback", params);
}

int32_t JsonRpcEndpoint::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMapSP			params) {
	fprintf(stdout, "--> recv_req: %s\n", method.c_str());
	fflush(stdout);

	if (id != -1) {
		if (method == "tblink.build-complete") {
			m_build_complete = true;
			IParamValMapSP result = m_transport->mkValMap();
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);
		} else if (method == "tblink.connect-complete") {
			m_connect_complete = true;
			IParamValMapSP result = m_transport->mkValMap();
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);
		} else if (method == "tblink.time-callback") {
			intptr_t callback_id = m_services->add_time_cb(
					params->getValT<IParamValInt>("time")->val_s());

			IParamValMapSP result = m_transport->mkValMap();
			result->setVal("callback-id",
					m_transport->mkValIntS(callback_id));
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);
		} else {
			fprintf(stdout, "Error: unhandled message\n");
		}
	} else {
		// Notifies
	}

	fprintf(stdout, "<-- recv_req: %s\n", method.c_str());
	fflush(stdout);

	return 0;
}

int32_t JsonRpcEndpoint::recv_rsp(
		intptr_t				id,
		IParamValMapSP			result,
		IParamValMapSP			error) {
	fprintf(stdout, "--> recv_rsp\n");
	fflush(stdout);
	m_rsp_m.insert({id, {result, error}});

	fprintf(stdout, "<-- recv_rsp\n");
	fflush(stdout);
	return 0;
}

std::pair<IParamValMapSP,IParamValMapSP> JsonRpcEndpoint::wait_rsp(intptr_t id) {
	std::map<intptr_t,std::pair<IParamValMapSP,IParamValMapSP>>::iterator it;
	std::pair<IParamValMapSP,IParamValMapSP> rsp;

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
