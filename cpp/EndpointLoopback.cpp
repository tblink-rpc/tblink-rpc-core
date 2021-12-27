/*
 * EndpointLoopback.cpp
 *
 *  Created on: Nov 26, 2021
 *      Author: mballance
 */

#include "Debug.h"
#include "EndpointLoopback.h"
#include "InterfaceInstLoopback.h"
#include "InterfaceType.h"

#define EN_DEBUG_ENDPOINT_LOOPBACK

#ifdef EN_DEBUG_ENDPOINT_LOOPBACK
#define DEBUG_ENTER(fmt, ...) \
	DEBUG_ENTER_BASE(EndpointLoopback, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) \
	DEBUG_LEAVE_BASE(EndpointLoopback, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
	DEBUG_BASE(EndpointLoopback, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

EndpointLoopback::EndpointLoopback() : m_primary(true) {
	m_peer = new EndpointLoopback(this);
	m_peer_u = EndpointLoopbackUP(m_peer);
}

EndpointLoopback::EndpointLoopback(EndpointLoopback *peer) :
		m_primary(false), m_peer(peer) {
}

EndpointLoopback::~EndpointLoopback() {
	// TODO Auto-generated destructor stub
}

int32_t EndpointLoopback::process_one_message() {
	return 0;
}

int32_t EndpointLoopback::send_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	return m_peer->peer_recv_req(method, id, params);
}

int32_t EndpointLoopback::peer_recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	return recv_req(method, id, params);
}

int32_t EndpointLoopback::send_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	return m_peer->peer_recv_rsp(id, result, error);
}

int32_t EndpointLoopback::peer_recv_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	return recv_rsp(id, result, error);
}

} /* namespace tblink_rpc_core */
