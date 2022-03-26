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
	m_build_check_complete = 0;
	m_connect_complete = 0;
	m_peer_connect_complete = 0;
	m_connect_check_complete = 0;

	m_comm_state = IEndpoint::Waiting;
	m_comm_mode = IEndpoint::Automatic;

	m_ifinst_id = 0;
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

int32_t EndpointBase::is_init() {
	DEBUG("is_init m_init=%d m_peer_init=%d", m_init, m_peer_init);
	if (m_init == -1 || m_peer_init == -1) {
		return -1;
	} else {
		return m_init && m_peer_init;
	}
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

int32_t EndpointBase::build_complete() {
	m_build_complete = 1;
	return 0;
}

int32_t EndpointBase::is_build_complete() {
	DEBUG("is_build_complete local=%d peer=%d",
			m_build_complete, m_peer_build_complete);
	if (!m_build_check_complete) {
		if (m_build_complete && m_peer_build_complete) {
			m_build_check_complete = build_complete_check();
		}
	}
	return m_build_check_complete;
}

int32_t EndpointBase::connect_complete() {
	m_connect_complete = 1;

	if (m_peer_connect_complete) {
		m_connect_check_complete = connect_complete_check();
	}
	return 0;
}

int32_t EndpointBase::is_connect_complete() {
	if (!m_connect_check_complete) {
		if (m_connect_complete && m_peer_connect_complete) {
			m_connect_check_complete = connect_complete_check();
		}
	}
	return m_connect_check_complete;
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
	auto it = m_local_ifc_types.find(name);

	if (it != m_local_ifc_types.end()) {
		return it->second;
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








	auto it = m_local_ifc_types.find(iftype->name());
	if (it != m_local_ifc_types.end()) {
		DEBUG("IfType %s exists", iftype->name().c_str());
	}

	DEBUG_ENTER("defineInterfaceType %s", iftype->name().c_str());
	m_local_ifc_types.insert({iftype->name(), iftype});
	m_local_ifc_type_pl.push_back(iftype);
	m_local_ifc_type_l.push_back(InterfaceTypeUP(iftype));

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
	fprintf(stdout, "peer_init:\n");
	for (auto it=args.begin(); it!=args.end(); it++) {
		fprintf(stdout, "    arg: %s\n", it->c_str());
		m_args.push_back(*it);
	}

	return 0;
}

int EndpointBase::peer_build_complete() {
	m_peer_build_complete = 1;

	return 0;
}

int32_t EndpointBase::build_complete_check() {
	int32_t ret = 1;

	// Ensure that both sides have the same types?
	return 1;
}

int EndpointBase::peer_connect_complete() {
	m_peer_connect_complete = 1;
	return 0;
}

int32_t EndpointBase::connect_complete_check() {
	int32_t ret = 1;
	DEBUG_ENTER("connect_complete_check");

	// Check results
	for (auto it=m_local_ifc_insts.begin();
			it!=m_local_ifc_insts.end(); it++) {
		ifinst_m_t::iterator peer_it;
		if ((peer_it=m_peer_ifc_insts.find(it->first)) != m_peer_ifc_insts.end()) {
			// Compare types
			IInterfaceInst *local_ifinst = it->second;
			IInterfaceInst *peer_ifinst = peer_it->second;

			if (local_ifinst->type()->name() != peer_ifinst->type()->name()) {
				fprintf(stdout, "local_ifinst::type=%p peer_ifinst::type=%p\n",
						local_ifinst->type(), peer_ifinst->type());
				fflush(stdout);
				last_error("Local interface %s is of type %s ; Peer is of type %s\n",
						local_ifinst->name().c_str(),
						local_ifinst->type()->name().c_str(),
						peer_ifinst->type()->name().c_str());
				ret = -1;
				break;
			}
			// Configure the local interface so it knows how to
			// talk to the peer.
			dynamic_cast<InterfaceInstBase *>(local_ifinst)->setRemoteId(
					dynamic_cast<InterfaceInstBase *>(peer_ifinst)->getLocalId());
		} else {
			// Instance doesn't exist
			last_error("Local interface %s does not have remote peer\n",
					it->second->name().c_str());
			ret = -1;
			break;
		}
	}

	// Check the opposite way
	if (ret == 1) {
		for (auto it=m_peer_ifc_insts.begin();
				it!=m_peer_ifc_insts.end(); it++) {
			ifinst_m_t::iterator local_it;
			if ((local_it=m_local_ifc_insts.find(it->first)) != m_local_ifc_insts.end()) {
				// Compare types
				IInterfaceInst *peer_ifinst = it->second;
				IInterfaceInst *local_ifinst = local_it->second;

				if (local_ifinst->type()->name() != peer_ifinst->type()->name()) {
					fprintf(stdout, "Error: Local interface %s is of type %s ; Peer is of type %s\n",
							local_ifinst->name().c_str(),
							local_ifinst->type()->name().c_str(),
							peer_ifinst->type()->name().c_str());
					ret = -1;
					break;
				}
			} else {
				// Instance doesn't exist
				fprintf(stdout, "Error: Local interface %s does not have remote peer\n",
						it->second->name().c_str());
				ret = -1;
				break;
			}
		}
	}

//	if (ret == 1) {
//		// Populate the local id->inst map
//		for (auto it=m_local_ifc_insts.begin();
//				it!=m_local_ifc_insts.end(); it++) {
//			InterfaceInstBase *ifinst =
//					dynamic_cast<InterfaceInstBase *>(it->second.get());
//			DEBUG("Add interface-id %lld", ifinst->getLocalId());
//			m_id2ifinst_m.insert({ifinst->getLocalId(), ifinst});
//		}
//	}

	DEBUG_LEAVE("connect_complete_check %d", ret);

	return ret;
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
