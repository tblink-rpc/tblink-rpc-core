/*
 * EndpointBase.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#include <stdarg.h>
#include "Debug.h"
#include "EndpointBase.h"
#include "EndpointEventBase.h"
#include "EndpointListenerBase.h"

#include "InterfaceInstBase.h"
#include "InterfaceType.h"
#include "InterfaceTypeBuilder.h"
#include "ParamValBool.h"
#include "ParamValInt.h"
#include "ParamValMap.h"
#include "ParamValStr.h"
#include "ParamValVec.h"

#define EN_DEBUG_ENDPOINT_BASE

#ifdef EN_DEBUG_ENDPOINT_BASE
#define DEBUG_ENTER(fmt, ...) \
	DEBUG_ENTER_BASE(EndpointBase, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) \
	DEBUG_LEAVE_BASE(EndpointBase, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
	DEBUG_BASE(EndpointBase, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

EndpointBase::EndpointBase() : m_flags(IEndpointFlags::Empty) {
	m_time = 0;
	m_time_precision = 0;

	m_init = 0;
	m_peer_init = 0;
	m_build_complete = 0;
	m_peer_build_complete = 0;
	m_connect_complete = 0;
	m_peer_connect_complete = 0;
	m_peer_local_check_complete = 0;

	m_comm_state = IEndpoint::Waiting;
	m_comm_mode = IEndpoint::Automatic;
}

EndpointBase::~EndpointBase() {
	// TODO Auto-generated destructor stub
}

IEndpointFlags EndpointBase::getFlags() {
	return m_flags;
}

void EndpointBase::setFlag(IEndpointFlags f) {
	m_flags |= f;
}

int32_t EndpointBase::init(IEndpointServices *services) {
	m_services = IEndpointServicesUP(services);

	if (m_services) {
		m_services->init(this);
	}

	return 0;
}

IEndpoint::comm_state_e EndpointBase::comm_state() {
	return m_comm_state;
}

void EndpointBase::update_comm_mode(
		IEndpoint::comm_mode_e m,
		IEndpoint::comm_state_e s) {
	DEBUG_ENTER("update_comm_mode");

	m_comm_mode = m;
	m_comm_state = s;

	DEBUG_LEAVE("update_comm_mode");
}

IEndpointListener *EndpointBase::addListener(const endpoint_ev_f &ev_f) {
	DEBUG_ENTER("addListener");
	EndpointListenerBase *l = new EndpointListenerBase(ev_f);
	m_listeners.push_back(IEndpointListenerUP(l));
	m_listeners_p.push_back(l);
	DEBUG_LEAVE("addListener %d", m_listeners_p.size());
	return l;
}

void EndpointBase::addListener(IEndpointListener *l) {
	DEBUG_ENTER("addListener");
	m_listeners_p.push_back(l);
	DEBUG_LEAVE("addListener %d", m_listeners_p.size());
}

void EndpointBase::removeListener(IEndpointListener *l) {
	for (auto it=m_listeners.begin(); it!=m_listeners.end(); it++) {
		if (it->get() == l) {
			m_listeners.erase(it);
			break;
		}
	}
	for (auto it=m_listeners_p.begin(); it!=m_listeners_p.end(); it++) {
		if (*it == l) {
			m_listeners_p.erase(it);
			break;
		}
	}
}

void EndpointBase::sendEvent(IEndpointEvent::kind_t kind) {
	EndpointEventBase ev(kind);
	sendEvent(&ev);
}

void EndpointBase::sendEvent(const IEndpointEvent *ev) {
	DEBUG_ENTER("sendEvent: %d", ev->kind());
	for (int32_t i=0; i<m_listeners_p.size(); i++) {
		IEndpointListener *l = m_listeners_p.at(i);
		DEBUG_ENTER("--> Send to %d", i);
		l->event(ev);
		DEBUG_ENTER("<-- Send to %d", i);
		if (i < m_listeners_p.size() && m_listeners_p.at(i) != l) {
			// list has changed
			i--;
		}
	}
	DEBUG_LEAVE("sendEvent: %d", ev->kind());
}

std::vector<std::string> EndpointBase::args() {
	if (m_services) {
		return m_services->args();
	} else {
		// TODO:
		return m_args;
	}
}

uint64_t EndpointBase::time() {
	if (m_services) {
		return m_services->time();
	} else {
		return m_time;
	}
}

int32_t EndpointBase::time_precision() {
	if (m_services) {
		return m_services->time_precision();
	} else {
		return m_time_precision;
	}
}

IInterfaceType *EndpointBase::findInterfaceType(
		const std::string		&name) {
	std::unordered_map<std::string,InterfaceTypeUP>::const_iterator it;

	if ((it=m_local_ifc_types.find(name)) != m_local_ifc_types.end()) {
		return it->second.get();
	} else {
		return 0;
	}
}

IInterfaceTypeBuilder *EndpointBase::newInterfaceTypeBuilder(
		const std::string		&name) {
	return new InterfaceTypeBuilder(name);
}

IInterfaceType *EndpointBase::defineInterfaceType(
		IInterfaceTypeBuilder	*type,
		IInterfaceImplFactory	*impl_factory,
		IInterfaceImplFactory	*impl_mirror_factory) {
	InterfaceTypeBuilder *builder = static_cast<InterfaceTypeBuilder *>(type);
	InterfaceType *iftype = builder->type();

	iftype->impl_f(impl_factory);
	iftype->impl_mirror_f(impl_mirror_factory);

	DEBUG_ENTER("defineInterfaceType %s", iftype->name().c_str());
	m_local_ifc_types.insert({iftype->name(), InterfaceTypeUP(iftype)});
	m_local_ifc_type_pl.push_back(iftype);

	DEBUG_LEAVE("defineInterfaceType %s", iftype->name().c_str());
	return iftype;
}

const std::vector<IInterfaceType *> &EndpointBase::getInterfaceTypes() {
	return m_local_ifc_type_pl;
}

const std::vector<IInterfaceType *> &EndpointBase::getPeerInterfaceTypes() {
	return m_peer_ifc_types_pl;
}

const std::vector<IInterfaceInst *> &EndpointBase::getInterfaceInsts() {
	return m_local_ifc_insts_pl;
}

const std::vector<IInterfaceInst *> &EndpointBase::getPeerInterfaceInsts() {
	return m_peer_ifc_insts_pl;
}

IParamValBool *EndpointBase::mkValBool(bool val) {
	return new ParamValBool();
}

IParamValInt *EndpointBase::mkValIntU(uint64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValInt *EndpointBase::mkValIntS(int64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValMap *EndpointBase::mkValMap() {
	return new ParamValMap();
}

IParamValStr *EndpointBase::mkValStr(const std::string &val) {
	return new ParamValStr(val);
}

IParamValVec *EndpointBase::mkValVec() {
	return new ParamValVec();
}

int EndpointBase::peer_init(
		int32_t								time_precision,
		const std::vector<std::string>		&args) {
	m_peer_init = 1;
	m_time_precision = time_precision;
	for (auto it=args.begin(); it!=args.end(); it++) {
		m_args.push_back(*it);
	}

	return 0;
}

int EndpointBase::peer_build_complete() {
	m_peer_build_complete = 1;

	return 0;
}

void EndpointBase::last_error(const char *fmt, ...) {
	va_list ap;
	char tmp[256];

	va_start(ap, fmt);
	vsnprintf(tmp, sizeof(tmp), fmt, ap);
	m_last_error = fmt;
	va_end(ap);
}

} /* namespace tblink_rpc_core */
