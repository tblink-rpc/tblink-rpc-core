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

EndpointLoopback::EndpointLoopback() : m_primary(true), m_cb_id(1) {
	m_peer = new EndpointLoopback(this);
	m_peer_u = EndpointLoopbackUP(m_peer);
	m_build_complete = false;
	m_peer_build_complete = false;
	m_connect_complete = false;
	m_peer_connect_complete = false;
}

EndpointLoopback::EndpointLoopback(EndpointLoopback *peer) :
		m_primary(false), m_peer(peer), m_cb_id(1) {
	m_build_complete = false;
	m_peer_build_complete = false;
	m_connect_complete = false;
	m_peer_connect_complete = false;
}

EndpointLoopback::~EndpointLoopback() {
	// TODO Auto-generated destructor stub
}

int32_t EndpointLoopback::init(
			IEndpointServices		*ep_services,
			IEndpointListener		*ep_listener) {
	EndpointBase::init(ep_services, ep_listener);

	if (m_primary) {
		m_peer->init(ep_services, ep_listener);
	}

	return 0;
}

int32_t EndpointLoopback::build_complete() {
	int32_t ret = 0;
	DEBUG_ENTER("build_complete primary=%d", m_primary);

	if (!m_build_complete && m_peer_build_complete) {
		// Trigger for end of build phase
		ret = process_build_complete();
	}

	m_build_complete = true;

	if (ret != -1) {
		// Notify peer that this side is complete
		ret = m_peer->peer_build_complete();
	}

	DEBUG_LEAVE("build_complete primary=%d", m_primary);
	return ret;
}

int32_t EndpointLoopback::peer_build_complete() {
	int32_t ret = 0;

	DEBUG_ENTER("peer_build_complete %d", m_primary);
	if (m_build_complete && !m_peer_build_complete) {
		// Trigger for end of build phase
		ret = process_build_complete();

		m_peer_build_complete = true;
	}

	m_peer_build_complete = true;

	// Send an event notifying that we've received a build-complete
	sendEvent(IEndpointEvent::Unknown);

	DEBUG_LEAVE("peer_build_complete %d", m_primary);
	return ret;
}

int32_t EndpointLoopback::process_build_complete() {
	// TODO: no real activity to undertake here
	return 0;
}

int32_t EndpointLoopback::is_build_complete() {
	DEBUG(":is_build_complete %d",
			(m_peer_build_complete && m_build_complete));
	return (m_peer_build_complete && m_build_complete);
}

int32_t EndpointLoopback::connect_complete() {
	int32_t ret = 0;
	DEBUG_ENTER("connect_complete");

	ret = m_peer->peer_connect_complete();

	if (!m_connect_complete && m_peer_connect_complete) {
		// Trigger for end of connect phase
		ret = process_connect_complete();
	}
	m_connect_complete = true;
	DEBUG_LEAVE("connect_complete");
	return ret;
}

int32_t EndpointLoopback::peer_connect_complete() {
	int32_t ret = 0;
	DEBUG_ENTER("peer_connect_complete primary=%d", m_primary);
	if (m_connect_complete && !m_peer_connect_complete) {
		// Trigger for end of connect phase
		ret = process_connect_complete();
	}
	m_peer_connect_complete = true;

	// Send an event notifying that we've received a connect-complete
	sendEvent(IEndpointEvent::Unknown);

	DEBUG_LEAVE("peer_connect_complete primary=%d", m_primary);
	return ret;
}

int32_t EndpointLoopback::is_connect_complete() {
	DEBUG_ENTER("is_connect_complete");
	DEBUG_LEAVE("is_connect_complete");
	return (m_connect_complete && m_peer_connect_complete);
}

int32_t EndpointLoopback::process_connect_complete() {
	// Confirm presence of interfaces
	// Connect peers together

	for (auto ifinst_it=m_ifinsts.begin();
			ifinst_it!=m_ifinsts.end(); ifinst_it++) {
		IInterfaceInst *peer_if = 0;
		for (auto pifinst_it=m_peer->getInterfaceInsts().begin();
				pifinst_it!=m_peer->getInterfaceInsts().end(); pifinst_it++) {
			if ((*ifinst_it)->name() == (*pifinst_it)->name()) {
				peer_if = *pifinst_it;
				break;
			}
		}

		if (peer_if) {
			fprintf(stdout, "Note: Matched %s\n", (*ifinst_it)->name().c_str());
		} else {
			fprintf(stdout, "Error: failed to find %s\n", (*ifinst_it)->name().c_str());
		}
	}
	return 1;
}

intptr_t EndpointLoopback::add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) {
	intptr_t cb_id = m_cb_id;

	m_cb_m.insert({cb_id, cb_f});

	m_cb_id += 1;

	m_services->add_time_cb(time, cb_id);
	return cb_id;
}

IInterfaceType *EndpointLoopback::findInterfaceType(
		const std::string		&name) {
	if (m_primary) {
		return EndpointBase::findInterfaceType(name);
	} else {
		return m_peer->findInterfaceType(name);
	}
}

IInterfaceTypeBuilder *EndpointLoopback::newInterfaceTypeBuilder(
		const std::string		&name) {
	return EndpointBase::newInterfaceTypeBuilder(name);
}

IInterfaceType *EndpointLoopback::defineInterfaceType(
		IInterfaceTypeBuilder	*type) {
	return EndpointBase::defineInterfaceType(type);
}

IInterfaceInst *EndpointLoopback::defineInterfaceInst(
		IInterfaceType			*type,
		const std::string		&inst_name,
		bool					is_mirror,
		const invoke_req_f		&req_f) {
	DEBUG_ENTER("defineInterfaceInst");
	InterfaceInstLoopback *ifinst = 0;
	ifinst = new InterfaceInstLoopback(
			this,
			static_cast<InterfaceType *>(type),
			inst_name,
			is_mirror,
			req_f);
	m_ifinst_m.insert({inst_name, InterfaceInstUP(ifinst)});
	m_ifinsts.push_back(ifinst);

	DEBUG_LEAVE("defineInterfaceInst");
	return ifinst;
}

const std::vector<IInterfaceType *> &EndpointLoopback::getInterfaceTypes() {
	return EndpointBase::getInterfaceTypes();
}

/**
 * Returns the available interface types registered by the
 * endpoint peer. Only valid after 'build' is complete
 */
const std::vector<IInterfaceType *> &EndpointLoopback::getPeerInterfaceTypes() {
	return m_peer->getInterfaceTypes();
}

/**
 * Returns the available interface instances registered by the
 * endpoint peer. Only valid after 'build' is complete
 */
const std::vector<IInterfaceInst *> &EndpointLoopback::getInterfaceInsts() {
	if (m_primary) {
		return EndpointBase::getInterfaceInsts();
	} else {
		return m_peer->getInterfaceInsts();
	}
}

const std::vector<IInterfaceInst *> &EndpointLoopback::getPeerInterfaceInsts() {
	DEBUG_ENTER("getPeerInterfaceInsts");
	DEBUG_LEAVE("getPeerInterfaceInsts");
	return m_peer->getInterfaceInsts();
}

} /* namespace tblink_rpc_core */
