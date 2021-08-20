/*
 * FifoMessageTransport.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#include "FifoMessageTransport.h"
#include "FifoMessageTransportEP.h"

namespace tblink_rpc_core {

FifoMessageTransport::FifoMessageTransport(
		bool is_blocking) : m_is_blocking(is_blocking),
				m_ep0(new FifoMessageTransportEP(this, 0)),
				m_ep1(new FifoMessageTransportEP(this, 1)) {
	// TODO Auto-generated constructor stub

}

FifoMessageTransport::~FifoMessageTransport() {
	// TODO Auto-generated destructor stub
}

FifoMessageTransportEP *FifoMessageTransport::ep(uint32_t id) {
	return (id==0)?m_ep0.get():m_ep1.get();
}

int32_t FifoMessageTransport::send_req(
		uint32_t				ep_id,
		const std::string		&method,
		intptr_t				req_id,
		IParamValMap			*params) {
	FifoMessageTransportEP *e = ep(ep_id);
	return e->recv_req(method, req_id, params);
}

int32_t FifoMessageTransport::send_notify(
		uint32_t				ep_id,
		const std::string		&method,
		IParamValMap			*params) {
	FifoMessageTransportEP *e = ep(ep_id);
	return -1;
}

int32_t FifoMessageTransport::send_rsp(
		uint32_t			ep_id,
		intptr_t			id,
		IParamValMap		*result,
		IParamValMap		*error) {
	FifoMessageTransportEP *e = ep(ep_id);
	return e->recv_rsp(id, result, error);
}

} /* namespace tblink_rpc_core */
