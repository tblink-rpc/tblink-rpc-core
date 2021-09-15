/*
 * FifoMessageTransportEndpointEP.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#include "ParamValBool.h"
#include "ParamValInt.h"
#include "ParamValMap.h"
#include "ParamValStr.h"
#include "ParamValVec.h"
#include "TransportMessageFifo.h"
#include "TransportMessageFifoUP.h"

namespace tblink_rpc_core {

TransportMessageFifoUP::TransportMessageFifoUP(
		TransportMessageFifo	*parent,
		uint32_t				id) :
			m_parent(parent), m_id(id),
			m_req_id(0) {
	// TODO Auto-generated constructor stub

}

TransportMessageFifoUP::~TransportMessageFifoUP() {
	// TODO Auto-generated destructor stub
}

void TransportMessageFifoUP::init(
		const recv_req_f &req_f,
		const recv_rsp_f &rsp_f) {
	m_req_f = req_f;
	m_rsp_f = rsp_f;
}

void TransportMessageFifoUP::shutdown() {

}

intptr_t TransportMessageFifoUP::send_req(
		const std::string		&method,
		IParamValMap			*params) {
	intptr_t req_id = m_req_id;
	m_req_id += 1;
	int32_t req_ret = m_parent->send_req(
			(m_id)?0:1,
			method,
			req_id,
			params);

	if (req_ret < 0) {
		return -1;
	} else {
		return req_id;
	}
}

int32_t TransportMessageFifoUP::send_notify(
		const std::string		&method,
		IParamValMap			*params) {
	return -1;
}

int32_t TransportMessageFifoUP::send_rsp(
		intptr_t			id,
		IParamValMap		*result,
		IParamValMap		*error) {
	return m_parent->send_rsp(
			(m_id)?0:1,
			id,
			result,
			error);
}

int32_t TransportMessageFifoUP::recv_rsp(
			intptr_t				id,
			IParamValMap			*result,
			IParamValMap			*error) {
	int32_t ret = 0; // m_rsp_f(id, result, error);
	fprintf(stdout, "recv_rsp\n");
	fflush(stdout);

	if (m_rsp_f && m_rsp_q.size() == 0) {
		ret = m_rsp_f(id, result, error);
	} else {
		// Save it for later
		m_rsp_q.push_back({id, result, error});
	}

	return ret;
}

int32_t TransportMessageFifoUP::poll(int32_t timeout_ms) {
	return -1;
}

/**
 * Waits for at least one message to be received
 */
int32_t TransportMessageFifoUP::await_msg() {
	int32_t ret = 0;

	fprintf(stdout, "[%d] await_msg: %d %d\n",
			m_id, m_req_q.size(), m_rsp_q.size());
	fflush(stdout);

	while (m_req_q.size() > 0) {
		const req_elem_t &req = m_req_q.front();
		m_req_f(
				std::get<0>(req),
				std::get<1>(req),
				std::get<2>(req));
		delete std::get<2>(req);
		m_req_q.erase(m_req_q.begin());
		ret = 1;
	}

	while (m_rsp_q.size() > 0) {
		const rsp_elem_t &rsp = m_rsp_q.front();
		m_rsp_f(
				std::get<0>(rsp),
				std::get<1>(rsp),
				std::get<2>(rsp));
		if (std::get<1>(rsp)) {
			delete std::get<1>(rsp);
		}
		if (std::get<2>(rsp)) {
			delete std::get<2>(rsp);
		}
		m_rsp_q.erase(m_rsp_q.begin());
		ret = 1;
	}

	return ret;
}

/**
 * Returns number of mismatched req/rsp pairs
 */
int32_t TransportMessageFifoUP::outstanding() {
	return 0;
}

IParamValBool *TransportMessageFifoUP::mkValBool(bool val) {
	return new ParamValBool(val);
}

IParamValInt *TransportMessageFifoUP::mkValIntU(uint64_t val) {
	return new ParamValInt(val);
}

IParamValInt *TransportMessageFifoUP::mkValIntS(int64_t val) {
	return new ParamValInt(val);
}

IParamValMap *TransportMessageFifoUP::mkValMap() {
	return new ParamValMap();
}

IParamValStr *TransportMessageFifoUP::mkValStr(const std::string &val) {
	return new ParamValStr(val);
}

IParamValVec *TransportMessageFifoUP::mkValVec() {
	return new ParamValVec();
}


int32_t TransportMessageFifoUP::recv_req(
			const std::string		&method,
			intptr_t				req_id,
			IParamValMap			*params) {
	int32_t req_v = 0;

	fprintf(stdout, "[%d] recv_req %s\n",
			m_id, method.c_str());
	fflush(stdout);

//	m_req_q.push_back({method, req_id, params});

	if (m_req_f && m_req_q.size() == 0) {
		m_req_f(method, req_id, params);

		// The lifetime of params ends once it's
		// been presented to the request function
		delete params;
	} else {
		// Save it for later
		m_req_q.push_back({method, req_id, params});
	}

	return req_v;
}

} /* namespace tblink_rpc_core */