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

int32_t EndpointLoopback::build_complete() {
	DEBUG_ENTER("build_complete");
	DEBUG_LEAVE("build_complete");
	return 1;
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
	if (m_primary) {
		return EndpointBase::newInterfaceTypeBuilder(name);
	} else {
		return m_peer->newInterfaceTypeBuilder(name);
	}
}

IInterfaceType *EndpointLoopback::defineInterfaceType(
		IInterfaceTypeBuilder	*type) {
	if (m_primary) {
		return EndpointBase::defineInterfaceType(type);
	} else {
		return m_peer->defineInterfaceType(type);
	}
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

/**
 * Returns the available interface types registered by the
 * endpoint peer. Only valid after 'build' is complete
 */
const std::vector<IInterfaceType *> &EndpointLoopback::getInterfaceTypes() {
	if (m_primary) {
		return EndpointBase::getInterfaceTypes();
	} else {
		return m_peer->getInterfaceTypes();
	}
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
