/*
 * EndpointBase.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

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

EndpointBase::EndpointBase() : m_services(0), m_listener(0) {
	// TODO Auto-generated constructor stub

}

EndpointBase::~EndpointBase() {
	// TODO Auto-generated destructor stub
}

int32_t EndpointBase::init(IEndpointServices *services) {
	m_services = services;

	if (m_services) {
		m_services->init(this);
	}

	return 0;
}

IEndpoint::comm_state_e EndpointBase::comm_state() {
	// TODO:
	return Released;
}

void EndpointBase::update_comm_mode(
		IEndpoint::comm_mode_e m,
		IEndpoint::comm_state_e s) {
	// TODO:
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
		return {};
	}
}

uint64_t EndpointBase::time() {
	if (m_services) {
		return m_services->time();
	} else {
		return 0;
	}
}

int32_t EndpointBase::time_precision() {
	return m_services->time_precision();
}

IInterfaceType *EndpointBase::findInterfaceType(
		const std::string		&name) {
	std::unordered_map<std::string,IInterfaceTypeUP>::const_iterator it;

	if ((it=m_iftype_m.find(name)) != m_iftype_m.end()) {
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
		IInterfaceTypeBuilder	*type) {
	InterfaceTypeBuilder *builder =
			static_cast<InterfaceTypeBuilder *>(type);
	InterfaceType *iftype = builder->type();
	m_iftype_m.insert({iftype->name(), IInterfaceTypeUP(iftype)});
	m_iftypes.push_back(iftype);

	return iftype;
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

} /* namespace tblink_rpc_core */
