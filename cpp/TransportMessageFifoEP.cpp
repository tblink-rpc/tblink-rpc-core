/*
 * FifoMessageTransportEndpointEP.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#include "TransportMessageFifoEP.h"

#include "ParamValBool.h"
#include "ParamValInt.h"
#include "ParamValMap.h"
#include "ParamValStr.h"
#include "ParamValVec.h"
#include "TransportMessageFifo.h"

namespace tblink_rpc_core {

TransportMessageFifoEP::TransportMessageFifoEP(
		TransportMessageFifo	*parent,
		uint32_t				id) :
			m_parent(parent), m_id(id),
			m_req_id(0) {
	// TODO Auto-generated constructor stub

}

TransportMessageFifoEP::~TransportMessageFifoEP() {
	// TODO Auto-generated destructor stub
}

void TransportMessageFifoEP::init(
		const recv_req_f &req_f,
		const recv_rsp_f &rsp_f) {
	m_req_f = req_f;
	m_rsp_f = rsp_f;
}

void TransportMessageFifoEP::shutdown() {

}

intptr_t TransportMessageFifoEP::send_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	int32_t req_ret = m_parent->send_req(
			(m_id)?0:1,
			method,
			id,
			params);

	return req_ret;
}

int32_t TransportMessageFifoEP::send_notify(
		const std::string		&method,
		IParamValMap			*params) {
	return -1;
}

int32_t TransportMessageFifoEP::send_rsp(
		intptr_t			id,
		IParamValMap		*result,
		IParamValMap		*error) {
	return m_parent->send_rsp(
			(m_id)?0:1,
			id,
			result,
			error);
}

int32_t TransportMessageFifoEP::process_one_message() {
	return 0;
}

int32_t TransportMessageFifoEP::recv_rsp(
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

/**
 * Waits for at least one message to be received
 */
int32_t TransportMessageFifoEP::await_msg() {
	int32_t ret = 0;

	fprintf(stdout, "[%d] await_msg: %d %d\n",
			m_id, (int)m_req_q.size(), (int)m_rsp_q.size());
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
int32_t TransportMessageFifoEP::outstanding() {
	return 0;
}

IParamValBool *TransportMessageFifoEP::mkValBool(bool val) {
	return new ParamValBool(val);
}

IParamValInt *TransportMessageFifoEP::mkValIntU(uint64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValInt *TransportMessageFifoEP::mkValIntS(int64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValMap *TransportMessageFifoEP::mkValMap() {
	return new ParamValMap();
}

IParamValStr *TransportMessageFifoEP::mkValStr(const std::string &val) {
	return new ParamValStr(val);
}

IParamValVec *TransportMessageFifoEP::mkValVec() {
	return new ParamValVec();
}


int32_t TransportMessageFifoEP::recv_req(
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
