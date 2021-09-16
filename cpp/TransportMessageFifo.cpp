/*
 * FifoMessageTransport.cpp
 *
 *  Created on: Aug 13, 2021
 *      Author: mballance
 */

#include "TransportMessageFifo.h"

#include "TransportMessageFifoEP.h"

namespace tblink_rpc_core {

TransportMessageFifo::TransportMessageFifo(
		bool is_blocking) : m_is_blocking(is_blocking),
				m_ep0(new TransportMessageFifoEP(this, 0)),
				m_ep1(new TransportMessageFifoEP(this, 1)) {
	m_ep0->parent(this);
	m_ep1->parent(this);
	m_ep0->id(0);
	m_ep1->id(1);
	// TODO Auto-generated constructor stub

}

TransportMessageFifo::TransportMessageFifo(
			TransportMessageFifoEP		*ep0,
			TransportMessageFifoEP		*ep1,
			bool 						is_blocking) :
				m_ep0(ep0), m_ep1(ep1), m_is_blocking(is_blocking) {
}

TransportMessageFifo::~TransportMessageFifo() {
	// TODO Auto-generated destructor stub
}

TransportMessageFifoEP *TransportMessageFifo::ep(uint32_t id) {
	return (id==0)?m_ep0.get():m_ep1.get();
}

int32_t TransportMessageFifo::send_req(
		uint32_t				ep_id,
		const std::string		&method,
		intptr_t				req_id,
		IParamValMap			*params) {
	TransportMessageFifoEP *e = ep(ep_id);
	return e->recv_req(method, req_id, params);
}

int32_t TransportMessageFifo::send_notify(
		uint32_t				ep_id,
		const std::string		&method,
		IParamValMap			*params) {
	TransportMessageFifoEP *e = ep(ep_id);
	return -1;
}

int32_t TransportMessageFifo::send_rsp(
		uint32_t			ep_id,
		intptr_t			id,
		IParamValMap		*result,
		IParamValMap		*error) {
	TransportMessageFifoEP *e = ep(ep_id);
	return e->recv_rsp(id, result, error);
}

} /* namespace tblink_rpc_core */
