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
	DEBUG_ENTER("send_req");
	int32_t ret = m_peer->peer_recv_req(method, id, params);
	DEBUG_LEAVE("send_req %d", ret);
	return ret;
}

int32_t EndpointLoopback::peer_recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	DEBUG_ENTER("peer_recv_req");
	int32_t ret = recv_req(method, id, params);
	DEBUG_LEAVE("peer_recv_req %d", ret);
	return ret;
}

int32_t EndpointLoopback::send_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	DEBUG_ENTER("send_rsp");
	int32_t ret = m_peer->peer_recv_rsp(id, result, error);
	DEBUG_LEAVE("send_rsp %d", ret);
	return ret;
}

int32_t EndpointLoopback::peer_recv_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	DEBUG_ENTER("peer_recv_rsp");
	int32_t ret = recv_rsp(id, result, error);
	DEBUG_LEAVE("peer_recv_rsp %d", ret);
	return ret;
}

} /* namespace tblink_rpc_core */
