/*
 * FifoMessageTransport.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#include "TransportMessageFifo.h"

#include "TransportMessageFifoUP.h"

namespace tblink_rpc_core {

TransportMessageFifo::TransportMessageFifo(
		bool is_blocking) : m_is_blocking(is_blocking),
				m_ep0(new TransportMessageFifoUP(this, 0)),
				m_ep1(new TransportMessageFifoUP(this, 1)) {
	// TODO Auto-generated constructor stub

}

TransportMessageFifo::~TransportMessageFifo() {
	// TODO Auto-generated destructor stub
}

TransportMessageFifoUP *TransportMessageFifo::ep(uint32_t id) {
	return (id==0)?m_ep0.get():m_ep1.get();
}

int32_t TransportMessageFifo::send_req(
		uint32_t				ep_id,
		const std::string		&method,
		intptr_t				req_id,
		IParamValMap			*params) {
	TransportMessageFifoUP *e = ep(ep_id);
	return e->recv_req(method, req_id, params);
}

int32_t TransportMessageFifo::send_notify(
		uint32_t				ep_id,
		const std::string		&method,
		IParamValMap			*params) {
	TransportMessageFifoUP *e = ep(ep_id);
	return -1;
}

int32_t TransportMessageFifo::send_rsp(
		uint32_t			ep_id,
		intptr_t			id,
		IParamValMap		*result,
		IParamValMap		*error) {
	TransportMessageFifoUP *e = ep(ep_id);
	return e->recv_rsp(id, result, error);
}

} /* namespace tblink_rpc_core */
