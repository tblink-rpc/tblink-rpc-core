/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include <stdarg.h>
#include "Debug.h"
#include "EndpointEventBase.h"
#include "EndpointMsgBase.h"
#include "EndpointListenerBase.h"
#include "InterfaceType.h"
#include "InterfaceTypeBuilder.h"
#include "MethodType.h"
#include "ParamValBool.h"
#include "ParamValInt.h"
#include "ParamValMap.h"
#include "ParamValStr.h"
#include "ParamValVec.h"

#define EN_DEBUG_ENDPOINT_MSG_BASE

#ifdef EN_DEBUG_ENDPOINT_MSG_BASE
#define DEBUG_ENTER(fmt, ...) \
	DEBUG_ENTER_BASE(EndpointMsgBase, fmt, ##__VA_ARGS__)
#define DEBUG_LEAVE(fmt, ...) \
	DEBUG_LEAVE_BASE(EndpointMsgBase, fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) \
	DEBUG_BASE(EndpointMsgBase, fmt, ##__VA_ARGS__)
#else
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#define DEBUG(fmt, ...)
#endif

namespace tblink_rpc_core {

EndpointMsgBase::EndpointMsgBase() {
	m_flags = IEndpointFlags::Empty;
	m_id = 1;

	m_call_id = 0;

	m_time = 0;
	m_time_precision = 0;
	m_run_until_event = 0;
	m_event_received = 0;

	m_init = 0;
	m_peer_init = 0;
	m_build_complete = 0;
	m_peer_build_complete = 0;
	m_connect_complete = 0;
	m_peer_connect_complete = 0;
	m_peer_local_check_complete = 0;
	m_callback_id = 0;
	m_pending_blocking_calls = 0;
	m_release_reqs = 0;
	m_wait_reqs = 0;
	m_comm_state = IEndpoint::Waiting;
	m_comm_mode = IEndpoint::Automatic;

	m_req_m.insert({"tblink.init", std::bind(
			&EndpointMsgBase::req_init, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.build-complete", std::bind(
			&EndpointMsgBase::req_build_complete, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.connect-complete", std::bind(
			&EndpointMsgBase::req_connect_complete, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.add-time-callback", std::bind(
			&EndpointMsgBase::req_add_time_callback, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.shutdown", std::bind(
			&EndpointMsgBase::req_shutdown, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.notify-callback", std::bind(
			&EndpointMsgBase::req_notify_callback, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-b", std::bind(
			&EndpointMsgBase::req_invoke_b, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-rsp-b", std::bind(
			&EndpointMsgBase::req_invoke_rsp_b, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-nb", std::bind(
			&EndpointMsgBase::req_invoke_nb, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.run-until-event", std::bind(
			&EndpointMsgBase::req_run_until_event, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.set-comm-state", std::bind(
			&EndpointMsgBase::req_set_comm_state, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.update-comm-mode", std::bind(
			&EndpointMsgBase::req_update_comm_mode, this,
			std::placeholders::_1,
			std::placeholders::_2)});
}

EndpointMsgBase::~EndpointMsgBase() {
	// TODO Auto-generated destructor stub
}

IEndpointFlags EndpointMsgBase::getFlags() {
	return m_flags;
}

void EndpointMsgBase::setFlag(IEndpointFlags f) {
	m_flags |= f;
}

int32_t EndpointMsgBase::init(IEndpointServices *services) {
	DEBUG_ENTER("EndpointMsgBase::init services=%p", services);

	m_services = IEndpointServicesUP(services);
	if (m_services) {
		m_services->init(this);
	}

	auto params = mkValMap();

	if (m_services) {
		auto args_p = mkValVec();
		for (auto arg : m_services->args()) {
			args_p->push_back(mkValStr(arg));
		}

		params->setVal("args", args_p);
		if (m_services) {
			params->setVal("time-units", mkValIntS(m_services->time_precision(), 32));
		}
	}

	m_init = 1;
	if (send_req("tblink.init", params) == -1) {
		m_last_error = "Transport send_req returned error status";
		return -1;
	}

	// TODO: listener

	// TODO: send init message

	DEBUG_LEAVE("EndpointMsgBase::init services=%p", services);
	return 0;
}

int32_t EndpointMsgBase::is_init() {
	DEBUG("is_init m_init=%d m_peer_init=%d", m_init, m_peer_init);
	if (m_init == -1 || m_peer_init == -1) {
		return -1;
	} else {
		return m_init && m_peer_init;
	}
}

IEndpoint::comm_state_e EndpointMsgBase::comm_state() {
	fprintf(stdout, "comm_state: %d\n", m_comm_state);
	fflush(stdout);
	return m_comm_state;
}

void EndpointMsgBase::update_comm_mode(
		comm_mode_e 		m,
		comm_state_e 		s) {
	DEBUG_ENTER("update_comm_mode");
	IParamValMap *params = mkValMap();

	params->setVal("comm-mode",
			mkValIntS(static_cast<int64_t>(m), 32));
	params->setVal("comm-state",
			mkValIntS(static_cast<int64_t>(s), 32));
	send_req("tblink.update-comm-mode", params);

	if (m_comm_state != s || m_comm_mode != m) {
		// Send update

	}
	DEBUG_LEAVE("update_comm_mode");
}

IEndpointListener *EndpointMsgBase::addListener(const endpoint_ev_f &ev_f) {
	DEBUG_ENTER("addListener");
	EndpointListenerBase *l = new EndpointListenerBase(ev_f);
	m_listeners.push_back(IEndpointListenerUP(l));
	m_listeners_p.push_back(l);
	DEBUG_LEAVE("addListener %d", m_listeners_p.size());
	return l;
}

void EndpointMsgBase::addListener(IEndpointListener *l) {
	DEBUG_ENTER("addListener");
	m_listeners_p.push_back(l);
	DEBUG_LEAVE("addListener %d", m_listeners_p.size());
}

void EndpointMsgBase::removeListener(IEndpointListener *l) {
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

void EndpointMsgBase::sendEvent(IEndpointEvent::kind_t kind) {
	EndpointEventBase ev(kind);
	sendEvent(&ev);
}

void EndpointMsgBase::sendEvent(const IEndpointEvent *ev) {
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


int32_t EndpointMsgBase::build_complete() {
	DEBUG_ENTER("build_complete");
	IParamValMap *params = mkValMap();

	// Pack locally-registered interface types and instances
	params->setVal("iftypes", pack_iftypes(m_local_ifc_types));
	params->setVal("ifinsts", pack_ifinsts(m_local_ifc_insts));
	if (m_services) {
		params->setVal("time-precision", mkValIntS(
				m_services->time_precision(), 32));
	} else {
		params->setVal("time-precision", mkValIntS(-9, 32));
	}

	m_build_complete = 1;
	if (send_req("tblink.build-complete", params) == -1) {
		return -1;
	}

	if (m_peer_build_complete == -1) {
		return -1;
	}

	DEBUG_LEAVE("build_complete");
	return 0;
}

int32_t EndpointMsgBase::is_build_complete() {
	DEBUG("is_build_complete local=%d peer=%d",
			m_build_complete, m_peer_build_complete);
	if (m_build_complete == -1 || m_peer_build_complete == -1) {
		return -1;
	} else {
		return m_build_complete && m_peer_build_complete;
	}
}

int32_t EndpointMsgBase::connect_complete() {
	IParamValMap *params = mkValMap();

	// TODO: should bail if build has failed?

	m_connect_complete = 1;

	// Pack the current set of instances present in the client
	params->setVal("ifinsts", pack_ifinsts(m_local_ifc_insts));

	if (send_req("tblink.connect-complete", params) == -1) {
		last_error("Failed to send connect-complete message");
		return -1;
	}

	if (m_peer_connect_complete == -1) {
		return -1;
	} else {
		return 0;
	}
}

int32_t EndpointMsgBase::is_connect_complete() {
	if (m_peer_local_check_complete == 0 &&
			m_connect_complete && m_peer_connect_complete) {

		// Check results
		m_peer_local_check_complete = 1;
		for (auto it=m_local_ifc_insts.begin();
				it!=m_local_ifc_insts.end(); it++) {
			std::unordered_map<std::string,InterfaceInstMsgTransportUP>::iterator peer_it;
			if ((peer_it=m_peer_ifc_insts.find(it->first)) != m_peer_ifc_insts.end()) {
				// Compare types
				InterfaceInstBase *local_ifinst = it->second.get();
				InterfaceInstBase *peer_ifinst = peer_it->second.get();

				if (local_ifinst->type()->name() != peer_ifinst->type()->name()) {
					fprintf(stdout, "local_ifinst::type=%p peer_ifinst::type=%p\n",
							local_ifinst->type(), peer_ifinst->type());
					fflush(stdout);
					fprintf(stdout, "Error: Local interface %s is of type %s ; Peer is of type %s\n",
							local_ifinst->name().c_str(),
							local_ifinst->type()->name().c_str(),
							peer_ifinst->type()->name().c_str());
					m_peer_local_check_complete = -1;
					break;
				}
			} else {
				// Instance doesn't exist
				fprintf(stdout, "Error: Local interface %s does not have remote peer\n",
						it->second->name().c_str());
				m_peer_local_check_complete = -1;
				break;
			}
		}

		// Check the opposite way
		if (m_peer_local_check_complete == 1) {
			for (auto it=m_peer_ifc_insts.begin();
					it!=m_peer_ifc_insts.end(); it++) {
				std::unordered_map<std::string,InterfaceInstMsgTransportUP>::iterator local_it;
				if ((local_it=m_local_ifc_insts.find(it->first)) != m_local_ifc_insts.end()) {
					// Compare types
					InterfaceInstBase *peer_ifinst = it->second.get();
					InterfaceInstBase *local_ifinst = local_it->second.get();

					if (local_ifinst->type()->name() != peer_ifinst->type()->name()) {
						fprintf(stdout, "Error: Local interface %s is of type %s ; Peer is of type %s\n",
								local_ifinst->name().c_str(),
								local_ifinst->type()->name().c_str(),
								peer_ifinst->type()->name().c_str());
						m_peer_local_check_complete = -1;
						break;
					}
				} else {
					// Instance doesn't exist
					fprintf(stdout, "Error: Local interface %s does not have remote peer\n",
							it->second->name().c_str());
					m_peer_local_check_complete = -1;
					break;
				}
			}
		}

		// TODO: notify listener if present
	}

	return m_peer_local_check_complete;
}

bool EndpointMsgBase::shutdown() {
//	if (m_state != IEndpoint::Shutdown && m_transport) {
		IParamValMap *params = mkValMap();

		if (send_req("tblink.shutdown", params) == -1) {
			return false;
		}

		// TODO: only wait a little bit for a response...
		//	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

		/* TODO
		m_transport->shutdown();
		 */

//		m_state = IEndpoint::Shutdown;
//	}

	return true;
}

intptr_t EndpointMsgBase::add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) {
	IParamValMap *params = mkValMap();
	params->setVal("time", mkValIntU(time, 64));
	intptr_t callback_id = m_callback_id;
	params->setVal("callback-id", mkValIntU(callback_id, 32));
	m_callback_id += 1;

	m_callback_m.insert({callback_id, cb_f});

	intptr_t id = send_req("tblink.add-time-callback", params);

#ifdef UNDEFINED
	rsp_t rsp = wait_rsp(id);

	if (rsp.first) {
		return callback_id;
	} else {
		fprintf(stdout, "Error: no response\n");
		// Error:
		return -1;
	}
#endif
}

std::vector<std::string> EndpointMsgBase::args() {
	if (m_services) {
		return m_services->args();
	} else {
		return m_args;
	}
}

uint64_t EndpointMsgBase::time() {
	if (m_services) {
		return m_services->time();
	} else {
		return m_time;
	}
}

void EndpointMsgBase::cancel_callback(intptr_t	callback_id) {
	IParamValMap *params = mkValMap();

	params->setVal("callback-id", mkValIntU(callback_id, 64));

	intptr_t id = send_req("tblink.cancel-callback", params);
}

/** Called by the environment to notify that
 *  a callback has occurred
 */
void EndpointMsgBase::notify_callback(intptr_t   callback_id) {
	IParamValMap *params = mkValMap();

	params->setVal("callback-id", mkValIntU(callback_id, 64));
	if (m_services) {
		params->setVal("time", mkValIntU(m_services->time(), 64));
	} else {
		params->setVal("time", mkValIntU(0, 64));
	}

	intptr_t id = send_req("tblink.notify-callback", params);

	// TODO: We don't really care about the response directly. Can
	// we let the system know to expect a response, but clear it out
	// once received?
#ifdef UNDEFINED
	rsp_t rsp = wait_rsp(id);
#endif
}

void EndpointMsgBase::last_error(const char *fmt, ...) {
	va_list ap;
	char tmp[128];

	va_start(ap, fmt);
	vsnprintf(tmp, sizeof(tmp), fmt, ap);
	m_last_error = fmt;
	va_end(ap);
}

IInterfaceType *EndpointMsgBase::findInterfaceType(
			const std::string		&name) {
	std::unordered_map<std::string,InterfaceTypeUP>::const_iterator it;

	if ((it=m_local_ifc_types.find(name)) != m_local_ifc_types.end()) {
		return it->second.get();
	} else {
		return 0;
	}
}

IInterfaceTypeBuilder *EndpointMsgBase::newInterfaceTypeBuilder(
			const std::string		&name) {
	return new InterfaceTypeBuilder(name);
}

IInterfaceType *EndpointMsgBase::defineInterfaceType(
			IInterfaceTypeBuilder	*type,
			IInterfaceInstFactory	*factory) {
	InterfaceTypeBuilder *builder =
			static_cast<InterfaceTypeBuilder *>(type);
	InterfaceType *if_type = builder->type();
	DEBUG_ENTER("defineInterfaceType %s", if_type->name().c_str());
	m_local_ifc_types.insert({if_type->name(), InterfaceTypeUP(if_type)});
//	m_local_ifc_type_l.push_back(InterfaceTypeUP(if_type));
	m_local_ifc_type_pl.push_back(if_type);

	DEBUG_LEAVE("defineInterfaceType %s", if_type->name().c_str());
	return if_type;
}

IInterfaceInst *EndpointMsgBase::defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			IInterfaceImpl			*impl) {
	DEBUG_ENTER("defineInterfaceInst: %s type=%s is_mirror=%d",
			inst_name.c_str(), type->name().c_str(), is_mirror);

	InterfaceInstMsgTransport *ifinst = new InterfaceInstMsgTransport(
			this,
			static_cast<InterfaceType *>(type),
			inst_name,
			is_mirror,
			impl);
	m_local_ifc_insts.insert({inst_name, InterfaceInstMsgTransportUP(ifinst)});
	m_local_ifc_insts_pl.push_back(ifinst);

	DEBUG_LEAVE("defineInterfaceInst: %s type=%s is_mirror=%d",
			inst_name.c_str(), type->name().c_str(), is_mirror);
	return ifinst;
}

const std::vector<IInterfaceType *> &EndpointMsgBase::getInterfaceTypes() {
	return m_local_ifc_type_pl;
}

const std::vector<IInterfaceType *> &EndpointMsgBase::getPeerInterfaceTypes() {
	return m_peer_ifc_types_pl;
}

const std::vector<IInterfaceInst *> &EndpointMsgBase::getInterfaceInsts() {
	return m_local_ifc_insts_pl;
}

const std::vector<IInterfaceInst *> &EndpointMsgBase::getPeerInterfaceInsts() {
	return m_peer_ifc_insts_pl;
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_init(
			intptr_t		id,
			IParamValMap 	*params) {
	DEBUG_ENTER("req_init (m_services=%p)", m_services.get());
	m_peer_init = 1;

	if (params->hasKey("time-units")) {
		m_time_precision = params->getValT<IParamValInt>("time-units")->val_s();
		IParamValVec *args_p = params->getValT<IParamValVec>("args");

		for (uint32_t i=0; i<args_p->size(); i++) {
			m_args.push_back(args_p->atT<IParamValStr>(i)->val());
		}
	}

	DEBUG_LEAVE("req_init");
	return {IParamValMapUP(mkValMap()), IParamValMapUP()};
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_build_complete(
		intptr_t				id,
		IParamValMap 			*params) {
	DEBUG_ENTER("req_build_complete");
	IParamValMap *result = 0;
	IParamValMap *error = 0;
	std::unordered_map<std::string,InterfaceTypeUP> iftypes;

	unpack_iftypes(
			m_peer_ifc_types,
			m_peer_ifc_types_pl,
			params->getValT<IParamValMap>("iftypes"));

	// Now, unpack ifinsts. We need to have a consistent
	// type system prior to doing so
	std::unordered_map<std::string,InterfaceInstUP> ifinsts;
	unpack_ifinsts(
			m_peer_ifc_insts,
			m_peer_ifc_insts_pl,
			params->getValT<IParamValMap>("ifinsts"));

	for (auto ifinst_it=m_peer_ifc_insts_pl.begin();
			ifinst_it!=m_peer_ifc_insts_pl.end(); ifinst_it++) {
		// Find the appropriate iftype
	}

#ifdef UNDEFINED
	for (std::unordered_map<std::string,InterfaceInstUP>::iterator
			it=ifinsts.begin();
			it!=ifinsts.end(); it++) {
		std::map<std::string,InterfaceInstBase*>::const_iterator m_it;
		if ((m_it=m_local_ifc_insts.find((*it)->name())) != m_local_ifc_insts.end()) {
			// Need to compare
		} else {
			// Add
			InterfaceInstBase *ifinst = it->release();
			m_local_ifc_insts.insert({
				ifinst->name(),
				ifinst
			});
			m_local_ifc_insts_l.push_back(InterfaceInstUP(ifinst));
			m_local_ifc_insts_pl.push_back(ifinst);
		}
	}
#endif

	m_time_precision = params->getValT<IParamValInt>("time-precision")->val_s();

	m_peer_build_complete = 1;

	if (!error) {
		result = mkValMap();
	}

	DEBUG_LEAVE("req_build_complete");
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_connect_complete(
		intptr_t				id,
		IParamValMap 			*params) {

	// The connect request conveys an updated set of interface instances
	/*
	m_local_ifc_insts.clear();
	m_local_ifc_insts_l.clear();
	m_local_ifc_insts_pl.clear();
	 */

	m_peer_ifc_insts.clear();
	m_peer_ifc_insts_pl.clear();
	unpack_ifinsts(
			m_peer_ifc_insts,
			m_peer_ifc_insts_pl,
			params->getValT<IParamValMap>("ifinsts"));

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	m_peer_connect_complete = 1;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_get_interface_types(
		intptr_t				id,
		IParamValMap 			*params) {
	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_add_time_callback(
		intptr_t				id,
		IParamValMap 			*params) {
	intptr_t callback_id = params->getValT<IParamValInt>("callback-id")->val_u();

	m_services->add_time_cb(
			params->getValT<IParamValInt>("time")->val_s(),
			callback_id);

	// Save the callback id for later
	if (!m_pending_time_cb.insert(callback_id).second) {
		// Error: duplicate ID
	}

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_notify_callback(
		intptr_t				id,
		IParamValMap 			*params) {
	intptr_t callback_id = params->getValT<IParamValInt>("callback-id")->val_s();
	m_time = params->getValT<IParamValInt>("time")->val_u();

	std::map<intptr_t,std::function<void()>>::const_iterator it;

	if ((it=m_callback_m.find(callback_id)) != m_callback_m.end()) {
		it->second();
		m_callback_m.erase(it);
	} else {
		fprintf(stdout, "Error: callback id %lld not present\n", callback_id);
	}

	m_event_received++;

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_shutdown(
		intptr_t				id,
		IParamValMap 			*params) {
	// Avoid recursive efforts to shutdown
	/*
	m_state = IEndpoint::Shutdown;
	 */

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_invoke_nb(
		intptr_t				id,
		IParamValMap 			*params) {
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	std::string method = params->getValT<IParamValStr>("method")->val();
	intptr_t call_id = params->getValT<IParamValInt>("call-id")->val_s();

	std::unordered_map<std::string,InterfaceInstMsgTransportUP>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {
		IMethodType *method_t = i_it->second->type()->findMethod(method);

		if (method_t) {
			IParamValVec *m_params = params->getValT<IParamValVec>("params");
			i_it->second->invoke_req(
					method_t,
					m_params,
					std::bind(&EndpointMsgBase::call_completion_nb,
							this,
							id,
							call_id,
							std::placeholders::_1));
			sendEvent(IEndpointEvent::InInvokeReqNB);
		} else {
			// TODO: error signaling
			fprintf(stdout, "Error: failed to find method %s\n", method.c_str());
			fflush(stdout);
		}
	} else {
		// TODO: error signaling
		fprintf(stdout, "Error: failed to find interface\n");
		fflush(stdout);
	}

	// An invoke request is considered an event
	m_event_received = true;

	IParamValMap *result = 0;
	IParamValMap *error = 0;

	// We will handle our own response
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_invoke_b(
		intptr_t				id,
		IParamValMap 			*params) {
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	std::string method = params->getValT<IParamValStr>("method")->val();
	uint64_t call_id = params->getValT<IParamValInt>("call-id")->val_u();

	std::unordered_map<std::string,InterfaceInstMsgTransportUP>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {
		IMethodType *method_t = i_it->second->type()->findMethod(method);

		if (method_t) {
			IParamValVec *m_params = params->getValT<IParamValVec>("params");

			m_release_reqs++;
			if (m_comm_mode == IEndpoint::Automatic) {
				m_comm_state = IEndpoint::Released;
			}
			sendEvent(IEndpointEvent::InInvokeReqB);

			i_it->second->invoke_req(
					method_t,
					m_params,
					std::bind(&EndpointMsgBase::call_completion_b,
							this,
							i_it->second.get(),
							call_id,
							std::placeholders::_1));
		} else {
			// TODO: error signaling
			fprintf(stdout, "Error: failed to find method\n");
			fflush(stdout);
		}
	} else {
		// TODO: error signaling
		fprintf(stdout, "Error: failed to find interface\n");
		fflush(stdout);
	}

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	// Send an 'ok' response. We'll separately send a response message
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_invoke_rsp_b(
		intptr_t				id,
		IParamValMap 			*params) {
	DEBUG_ENTER("req_invoke_rsp_b");
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	intptr_t call_id = params->getValT<IParamValInt>("call-id")->val_u();
	IParamVal *retval = 0;

	if (params->hasKey("return")) {
		retval = params->getVal("return");
	}

	auto rsp_it = m_outbound_invoke_m.find(call_id);

	if (rsp_it != m_outbound_invoke_m.end()) {
		rsp_it->second(retval);
		m_outbound_invoke_m.erase(rsp_it);
	} else {
		fprintf(stdout, "TbLink Error: unexpected invoke_b response %lld\n", call_id);
	}

	sendEvent(IEndpointEvent::InInvokeRspB);

#ifdef UNDEFINED
	std::unordered_map<std::string,InterfaceInstMsgTransportUP>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {

		i_it->second->invoke_b_rsp(
				call_id,
				retval);
	} else {
		// TODO: error signaling
		fprintf(stdout, "Error: failed to find interface\n");
		fflush(stdout);
	}
#endif

	// A blocking invoke response counts as an event
	m_event_received = true;

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	DEBUG_LEAVE("req_invoke_rsp_b");

	// Send an 'ok' response.
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_run_until_event(
		intptr_t				id,
		IParamValMap 			*params) {
	m_run_until_event++;

	// Tell the environment that it's free to run
//	m_services->run_until_event();

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_set_comm_state(
		intptr_t				id,
		IParamValMap 			*params) {
	DEBUG_ENTER("req_set_comm_state");

	sendEvent(IEndpointEvent::Unknown);

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	DEBUG_LEAVE("req_set_comm_state");
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgBase::rsp_t EndpointMsgBase::req_update_comm_mode(
		intptr_t				id,
		IParamValMap 			*params) {
	DEBUG_ENTER("req_update_comm_mode");
	IEndpoint::comm_mode_e m  = (IEndpoint::comm_mode_e)params->getValT<IParamValInt>("comm-mode")->val_s();
	IEndpoint::comm_state_e s = (IEndpoint::comm_state_e)params->getValT<IParamValInt>("comm-state")->val_s();

	m_comm_mode = m;
	m_comm_state = s;

	sendEvent(IEndpointEvent::Unknown);

	IParamValMap *result = mkValMap();
	IParamValMap *error = 0;

	DEBUG_LEAVE("req_update_comm_mode");
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}


void EndpointMsgBase::call_completion_nb(
			intptr_t		id,
			intptr_t		call_id,
			IParamVal		*retval) {
	IParamValMap *result = mkValMap();

	// Must attach call-id to this
	result->setVal("call-id", mkValIntS(call_id, 64));

	if (retval) {
		result->setVal("return", retval);
	}
	IParamValMap *error = 0;
	send_rsp(id, result, error);
}

void EndpointMsgBase::call_completion_b(
		InterfaceInstBase		*ifinst,
		intptr_t				call_id,
		IParamVal				*retval) {
	DEBUG_ENTER("call_completion_b");
	IParamValMap *params = mkValMap();

	params->setVal("ifinst", mkValStr(ifinst->name()));
	params->setVal("call-id", mkValIntU(call_id, 64));

	if (retval) {
		params->setVal("return", retval);
	}

	// Drop into 'wait' mode until we confirm
	// receipt of the ack
	if (m_comm_mode == Automatic) {
		m_comm_state = IEndpoint::Waiting;
	}
//	sendEvent(IEndpointEvent::Unknown);

	m_release_reqs--;

	sendEvent(IEndpointEvent::OutInvokeReqB);

	intptr_t id = send_req(
			"tblink.invoke-rsp-b",
			params,
			std::bind(
					&EndpointMsgBase::rsp_call_completion_b,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3));

	// Let the services know that we've encountered a
	// time-based event
//	m_services->hit_event();

	// Give the endpoint services a chance to decide how
	// best to proceed
//	m_services->idle();
	DEBUG_LEAVE("call_completion_b");
}

void EndpointMsgBase::rsp_call_completion_b(
			intptr_t			rsp_id,
			IParamValMap		*result,
			IParamValMap		*error) {
	DEBUG_ENTER("rsp_call_completion_b m_release_reqs=%d", m_release_reqs);

	if (m_comm_mode == Automatic) {
		if (m_release_reqs) {
			m_comm_state = IEndpoint::Released;
		} else {
			m_comm_state = IEndpoint::Waiting;
		}
	}
	sendEvent(IEndpointEvent::Unknown);

	DEBUG_LEAVE("rsp_call_completion_b");
}

int32_t EndpointMsgBase::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	DEBUG_ENTER("recv_req method=%s", method.c_str());


	if (id != -1) {
		std::map<std::string,req_func_t>::const_iterator it;

		if ((it=m_req_m.find(method)) != m_req_m.end()) {
			EndpointMsgBase::rsp_t rsp = it->second(id, params);

			// Note: if neither result not error are non-null,
			// then the method is handling its own response
			if (rsp.first || rsp.second) {
				send_rsp(
					id,
					rsp.first.release(),
					rsp.second.release());
			}

			/*
			if (m_state == IEndpoint::Shutdown && prev_state != IEndpoint::Shutdown) {
				m_services->shutdown();
			}
			 */
		} else {
			fprintf(stdout, "Error: unhandled message %s\n", method.c_str());
			fflush(stdout);
		}
	} else {
		// Notifies
	}

	// Issue an event
	sendEvent(IEndpointEvent::Unknown);

	DEBUG_LEAVE("recv_req method=%s", method.c_str());
	return 0;
}

int32_t EndpointMsgBase::recv_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	DEBUG_ENTER("recv_rsp");

	// We take ownership of result and error from the transport
	std::unordered_map<intptr_t,response_f>::iterator it;

	if ((it=m_rsp_m.find(id)) != m_rsp_m.end()) {
		it->second(id, result, error);
		m_rsp_m.erase(it);
	}

	sendEvent(IEndpointEvent::Unknown);

	DEBUG_LEAVE("recv_rsp");
	return 0;
}

intptr_t EndpointMsgBase::send_req(
		const std::string 	&method,
		IParamValMap		*params) {
	DEBUG_ENTER("send_req: %s", method.c_str());
	intptr_t id = m_id;
	m_id += 1;

	intptr_t ret = send_req(method, id, params);
	DEBUG_LEAVE("send_req: %s", method.c_str());
	return ret;
}

intptr_t EndpointMsgBase::send_req(
		const std::string 	&method,
		IParamValMap		*params,
		const response_f	&rsp_f) {
	DEBUG_ENTER("send_req(rsp_f): %s", method.c_str());
	// Insert a placeholder for the response we will receive
	intptr_t id = m_id;
	m_id += 1;

	m_rsp_m.insert({id, rsp_f});

	intptr_t ret = send_req(method, id, params);

	DEBUG_LEAVE("send_req(rsp_f): %s", method.c_str());
	return ret;
}

IParamValBool *EndpointMsgBase::mkValBool(bool val) {
	return new ParamValBool(val);
}

IParamValInt *EndpointMsgBase::mkValIntU(uint64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValInt *EndpointMsgBase::mkValIntS(int64_t val, int32_t width) {
	return new ParamValInt(val);
}

IParamValMap *EndpointMsgBase::mkValMap() {
	return new ParamValMap();
}

IParamValStr *EndpointMsgBase::mkValStr(const std::string &val) {
	return new ParamValStr(val);
}

IParamValVec *EndpointMsgBase::mkValVec() {
	return new ParamValVec();
}

EndpointMsgBase::rsp_t EndpointMsgBase::wait_rsp(intptr_t id) {
#ifdef UNDEFINED
	std::map<intptr_t,rspq_elem_t>::iterator it;
	std::pair<IParamValMap*,IParamValMap*> rsp;

	DEBUG_ENTER("wait_rsp");

	// Poll waiting
	while ((it=m_rsp_m.find(id)) != m_rsp_m.end() &&
			!it->second.second.first &&
			!it->second.second.second) {

		DEBUG_ENTER("wait_rsp --> await_msg");
		int32_t ret = m_transport->await_msg();
		DEBUG_LEAVE("wait_rsp await_msg ") << ret;

		if (ret == -1) {
			// Failure
			break;
		}
	}

	/*
	if (it != m_rsp_m.end()) {
		rsp = it->second.second;
		m_rsp_m.erase(it);
		DEBUG_LEAVE("wait_rsp");
		return std::make_pair(IParamValMapUP(rsp.first), IParamValMapUP(rsp.second));
	} else {
		DEBUG_LEAVE("wait_rsp");
		return std::make_pair(IParamValMapUP(), IParamValMapUP());
	}
	 */
#endif
}

int32_t EndpointMsgBase::invoke(
		InterfaceInstBase		*ifinst,
		IMethodType 			*method,
		IParamValVec 			*params,
		const invoke_rsp_f		&completion_f) {
	DEBUG_ENTER("invoke");
	int ret = 0;
	intptr_t call_id = m_call_id;
	m_call_id += 1;

	IParamValMap *r_params = mkValMap();
	r_params->setVal("ifinst", mkValStr(ifinst->name()));
	r_params->setVal("method", mkValStr(method->name()));
	r_params->setVal("call-id", mkValIntU(call_id, 64));
	r_params->setVal("params", params);

	// Notify clients that communication state is changing
	if (m_comm_mode == IEndpoint::Automatic) {
		if (m_comm_state == IEndpoint::Released) {
			DEBUG("Back to Waiting state");
			m_comm_state = IEndpoint::Waiting;
			sendEvent(IEndpointEvent::Unknown);
		}
	} else {
		DEBUG("Retain state due to Automatic mode");
	}

	m_outbound_invoke_m.insert({call_id, completion_f});

	if (method->is_blocking()) {
		ret = send_req(
				"tblink.invoke-b",
				r_params);
		sendEvent(IEndpointEvent::OutInvokeReqB);
	} else {
		sendEvent(IEndpointEvent::OutInvokeReqNB);
		ret = send_req(
				"tblink.invoke-nb",
				r_params,
				std::bind(&EndpointMsgBase::invoke_nb_rsp, this,
						std::placeholders::_1,
						std::placeholders::_2,
						std::placeholders::_3));
	}

	DEBUG_LEAVE("invoke");

	return ret;
}

void EndpointMsgBase::invoke_nb_rsp(
			intptr_t			msg_id,
			IParamValMap		*result,
			IParamValMap		*error) {
	intptr_t call_id = result->getValT<IParamValInt>("call-id")->val_s();
	auto rsp_it = m_outbound_invoke_m.find(call_id);

	if (rsp_it != m_outbound_invoke_m.end()) {
		IParamVal *retval = 0;

		if (result->hasKey("return")) {
			retval = result->getVal("return");
		}

		rsp_it->second(retval);
		sendEvent(IEndpointEvent::InInvokeRspNB);

		m_outbound_invoke_m.erase(rsp_it);
	} else {
		fprintf(stdout, "TbLink Error: unexpected invoke response for %lld\n", call_id);
	}
}

IParamValMap *EndpointMsgBase::pack_iftypes(
		const std::unordered_map<std::string,InterfaceTypeUP> &iftypes) {
	DEBUG_ENTER("pack_iftypes");
	IParamValMap *ret = mkValMap();

	for (std::unordered_map<std::string,InterfaceTypeUP>::const_iterator
			it=iftypes.begin();
			it!=iftypes.end(); it++) {
		DEBUG("Type: %s", it->first.c_str());
		IParamValMap *iftype = mkValMap();
		iftype->setVal("name", mkValStr(it->second->name()));
		IParamValMap *methods = mkValMap();
		for (std::vector<IMethodType *>::const_iterator
				m_it=it->second->methods().begin();
				m_it!=it->second->methods().end(); m_it++) {
			IParamValMap *method = mkValMap();

			DEBUG("Method: %s", (*m_it)->name().c_str());
			DEBUG("  rtype: %p", (*m_it)->rtype());

			IParamValMap *signature = mkValMap();

			if ((*m_it)->rtype()) {
				IParamValMap *rtype = pack_type((*m_it)->rtype());
				signature->setVal("rtype", rtype);
			}

			IParamValVec *params = mkValVec();
			for (auto p : (*m_it)->params()) {
				IParamValMap *p_m = mkValMap();
				p_m->setVal("name", mkValStr(p->name()));
				p_m->setVal("type", pack_type(p->type()));
				params->push_back(p_m);
			}
			signature->setVal("parameters", params);
			method->setVal("signature", signature);

			method->setVal("is-export", mkValBool((*m_it)->is_export()));
			method->setVal("is-blocking", mkValBool((*m_it)->is_blocking()));

			methods->setVal((*m_it)->name(), method);
		}
		iftype->setVal("methods", methods);

		ret->setVal(it->second->name(), iftype);
	}

	DEBUG_LEAVE("pack_iftypes");

	return ret;
}

IParamValMap *EndpointMsgBase::pack_ifinsts(
		const std::unordered_map<std::string, InterfaceInstMsgTransportUP> &ifinsts) {
	IParamValMap *ret = mkValMap();

	for (auto it=ifinsts.begin(); it!=ifinsts.end(); it++) {
		IParamValMap *ifinst = mkValMap();
		ifinst->setVal("type", mkValStr(it->second->type()->name()));
		ifinst->setVal("is-mirror", mkValBool(it->second->is_mirror()));
		ret->setVal(it->second->name(), ifinst);
	}

	return ret;
}

static std::unordered_map<TypeE, std::string> kind2str_m = {
		{TypeE::Bool, "bool"},
		{TypeE::Int, "int"},
		{TypeE::Map, "map"},
		{TypeE::Str, "str"},
		{TypeE::Vec, "vec"}
};

IParamValMap *EndpointMsgBase::pack_type(IType *t) {
	DEBUG_ENTER("pack_type: t=%p kind=%d", t, t->kind());
	IParamValMap *ret = mkValMap();

	ret->setVal("kind", mkValStr(
			kind2str_m.find(t->kind())->second));

	switch (t->kind()) {
	case TypeE::Int: {
		ITypeInt *ti = dynamic_cast<ITypeInt *>(t);
		ret->setVal("is-signed", mkValBool(ti->is_signed()));
		ret->setVal("width", mkValIntU(ti->width(), 32));
	} break;
	case TypeE::Map: {
		ITypeMap *tm = dynamic_cast<ITypeMap *>(t);
		ret->setVal("key-type", pack_type(tm->key_t()));
		ret->setVal("elem-type", pack_type(tm->elem_t()));
	} break;
	case TypeE::Vec: {
		ITypeVec *tv = dynamic_cast<ITypeVec *>(t);
		ret->setVal("elem-type", pack_type(tv->elem_t()));
	} break;
	}

	DEBUG_LEAVE("pack_type: t=%p", t);
	return ret;
}

void EndpointMsgBase::unpack_iftypes(
		std::unordered_map<std::string, InterfaceTypeUP>	&iftypes,
		std::vector<IInterfaceType *>						&iftypes_l,
		IParamValMap 										*iftypes_p) {
	DEBUG_ENTER("unpack_iftypes");
	std::vector<InterfaceTypeUP> ret;

	for (std::set<std::string>::const_iterator
			k_it=iftypes_p->keys().begin();
			k_it!=iftypes_p->keys().end(); k_it++) {
		DEBUG("iftype: %s", k_it->c_str());
		IParamValMap *iftype_v = iftypes_p->getValT<IParamValMap>(*k_it);
		InterfaceType *iftype = new InterfaceType(*k_it);
		IParamValMap *methods = iftype_v->getValT<IParamValMap>("methods");
		for (std::set<std::string>::const_iterator
				k_it=methods->keys().begin();
				k_it!=methods->keys().end(); k_it++) {
			IParamValMap *method_v = methods->getValT<IParamValMap>(*k_it);
			MethodType *method = new MethodType(
					(*k_it),
					0, // TODO: id
					0, // TODO: rtype
					// Note: we reverse the direction declared by the peer
					!method_v->getValT<IParamValBool>("is-export")->val(),
					method_v->getValT<IParamValBool>("is-blocking")->val());

			// TODO: parameters

			iftype->addMethod(method);
		}

		iftypes.insert({*k_it, InterfaceTypeUP(iftype)});
		iftypes_l.push_back(iftype);
	}

	DEBUG_LEAVE("unpack_iftypes");
}

void EndpointMsgBase::unpack_ifinsts(
		std::unordered_map<std::string,InterfaceInstMsgTransportUP>	&ifinsts,
		std::vector<IInterfaceInst*>								&ifinsts_l,
		IParamValMap 												*ifinsts_p) {
	std::vector<InterfaceInstUP> ret;
	DEBUG_ENTER("unpack_ifinsts");

	if (ifinsts_p) {
		for (std::set<std::string>::const_iterator
				k_it=ifinsts_p->keys().begin();
				k_it!=ifinsts_p->keys().end(); k_it++) {
			IParamValMap *ifinst_v = ifinsts_p->getValT<IParamValMap>(*k_it);
			std::unordered_map<std::string,InterfaceTypeUP>::const_iterator it;
			std::string tname = ifinst_v->getValT<IParamValStr>("type")->val();
			bool is_mirror = ifinst_v->getValT<IParamValBool>("is-mirror")->val();
			InterfaceType *type = 0;
			if ((it=m_peer_ifc_types.find(tname)) != m_peer_ifc_types.end()) {
				type = it->second.get();
			} else {
				fprintf(stdout, "Error: failed to find type %s\n", tname.c_str());
				fflush(stdout);
				for (auto t_it=m_peer_ifc_types.begin();
						t_it!=m_peer_ifc_types.end(); t_it++) {
					fprintf(stdout, "Type: %s\n", t_it->first.c_str());
				}
			}

			fprintf(stdout, "InterfaceInst %s: type=%p\n",
					k_it->c_str(), type);
			fflush(stdout);

			InterfaceInstMsgTransport *ifinst = new InterfaceInstMsgTransport(
					this,
					type,
					*k_it,
					is_mirror);
			ifinsts.insert({*k_it, InterfaceInstMsgTransportUP(ifinst)});
			ifinsts_l.push_back(ifinst);
		}
	} else {
		fprintf(stdout, "Error: unpack_ifinsts: null ifinsts_p\n");
	}
	DEBUG_LEAVE("unpack_ifinsts");
}

} /* namespace tblink_rpc_core */
