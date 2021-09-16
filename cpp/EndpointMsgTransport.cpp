/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include "EndpointMsgTransport.h"

#include "glog/logging.h"
#include "InterfaceType.h"
#include "InterfaceTypeBuilder.h"
#include "MethodType.h"

#define EN_DEBUG_JSON_RPC_ENDPOINT

#ifdef EN_DEBUG_JSON_RPC_ENDPOINT
#define DEBUG_ENTER(fmt, ...) \
	VLOG(1) << "--> JsonRpcEndpoint::" << fmt
#define DEBUG_LEAVE(fmt, ...) \
	VLOG(1) << "<-- JsonRpcEndpoint::" << fmt
#define DEBUG(fmt, ...) \
	VLOG(1) << "JsonRpcEndpoint: " << fmt
#else
#define DEBUG(fmt, ...)
#define DEBUG_ENTER(fmt, ...)
#define DEBUG_LEAVE(fmt, ...)
#endif

namespace tblink_rpc_core {

EndpointMsgTransport::EndpointMsgTransport(
		IEndpoint::Type		type,
		IEndpointServices	*services) {
	m_type = type;
	m_services  = services;
	m_transport = 0;

	m_state = IEndpoint::Init;
	m_time = 0;
	m_time_precision = 0;
	m_event_received = 0;


	m_services->init(this);

	m_build_complete = false;
	m_connect_complete = false;
	m_callback_id = 0;

	m_req_m.insert({"tblink.build-complete", std::bind(
			&EndpointMsgTransport::req_build_complete, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.connect-complete", std::bind(
			&EndpointMsgTransport::req_connect_complete, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.add-time-callback", std::bind(
			&EndpointMsgTransport::req_add_time_callback, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.shutdown", std::bind(
			&EndpointMsgTransport::req_shutdown, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.notify-callback", std::bind(
			&EndpointMsgTransport::req_notify_callback, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-b", std::bind(
			&EndpointMsgTransport::req_invoke_b, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-rsp-b", std::bind(
			&EndpointMsgTransport::req_invoke_rsp_b, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-nb", std::bind(
			&EndpointMsgTransport::req_invoke_nb, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.run-until-event", std::bind(
			&EndpointMsgTransport::req_run_until_event, this,
			std::placeholders::_1,
			std::placeholders::_2)});
}

EndpointMsgTransport::~EndpointMsgTransport() {
	// TODO Auto-generated destructor stub
}

void EndpointMsgTransport::init(ITransport *transport) {
	m_transport = transport;
	m_transport->init(
			std::bind(
					&EndpointMsgTransport::recv_req,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3),
			std::bind(
					&EndpointMsgTransport::recv_rsp,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3)
					);
}

bool EndpointMsgTransport::build_complete() {
	IParamValMap *params = m_transport->mkValMap();

	// Pack locally-registered interface types and instances
	params->setVal("iftypes", pack_iftypes());
	params->setVal("ifinsts", pack_ifinsts());
	params->setVal("time-precision", m_transport->mkValIntS(
			m_services->time_precision(), 32));

	intptr_t id = send_req(
			"tblink.build-complete",
			params,
			(m_type == Active));

	/*
	if (m_type == Active) {
		rsp_t rsp = wait_rsp(id);

		// Wait for a build-complete request
		while (!m_build_complete) {
			int ret = m_transport->poll(1000);

			if (ret == -1) {
				return false;
			}
		}

		if (m_state != IEndpoint::Shutdown) {
			m_state = IEndpoint::Built;
		}
	} else {
		// Assume the response will arrive at
		// some point
	}
	 */

	return true;
}

bool EndpointMsgTransport::connect_complete() {
	IParamValMap *params = m_transport->mkValMap();

	intptr_t id = send_req(
			"tblink.connect-complete",
			params,
			(m_type == Active));

	/*
	if (m_type == Active) {
		rsp_t rsp = wait_rsp(id);

		while (!m_connect_complete) {
			int ret = m_transport->poll(1000);

			if (ret == -1) {
				return false;
			}
		}

		if (m_state != IEndpoint::Shutdown) {
			m_state = IEndpoint::Connected;
		}

		return true;
	} else {
		// Passive endpoint

		return (id != -1);
	}
	 */
}

bool EndpointMsgTransport::shutdown() {
	if (m_state != IEndpoint::Shutdown && m_transport) {
		IParamValMap *params = m_transport->mkValMap();

		intptr_t id = send_req(
				"tblink.shutdown",
				params,
				false // Mark the request as not being actively awaited
				);

		// TODO: only wait a little bit for a response...
		//	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

		m_transport->shutdown();

		m_state = IEndpoint::Shutdown;
	}

	return true;
}

int32_t EndpointMsgTransport::run_until_event() {
	int32_t ret = 0;

	m_event_received = 0;

	// Send a run request to the peer
	IParamValMap *params = m_transport->mkValMap();
	intptr_t id = m_transport->send_req(
			"tblink.run-until-event",
			params);

	// Process messages until we receive an event notification
	while (m_event_received == 0) {
		ret = m_transport->await_msg();

		std::map<intptr_t,rspq_elem_t>::const_iterator it;
		if (id > 0 && (it=m_rsp_m.find(id)) != m_rsp_m.end()) {

			// Clear out the response
			m_rsp_m.erase(it);
			if (it->second.second.first) {
				delete it->second.second.first;
			}
			if (it->second.second.second) {
				delete it->second.second.second;
			}
			id = -1;
		}

		if (ret < 0) {
			break;
		}
	}

	return ret;
}

int32_t EndpointMsgTransport::await_req() {
	int32_t ret = 0;

	while ((ret=m_transport->await_msg()) == 0) {
		;
	}

	return ret;
}

int32_t EndpointMsgTransport::yield() {
	DEBUG_ENTER("yield");

	// If we have outstanding messages, or we have
	// no pending time-consuming operations, then
	// wait for longer. Otherwise, just check if
	// there is data.

	/*
	int32_t timeout;
	if (m_transport->outstanding()) {
		timeout = 1000;
	} else {
		timeout = 0;
	}

	int32_t ret = m_transport->poll(timeout);

	if (ret == -1) {
		DEBUG_LEAVE("yield -1");
		return ret;
	} else if (m_state == IEndpoint::Shutdown) {
		return 0;
	} else {
		if (m_transport->outstanding()) {
			ret = 1;
		}
		DEBUG_LEAVE("yield %d,%d", ret, m_transport->outstanding());
		return ret;
	}
	 */
}

int32_t EndpointMsgTransport::yield_blocking() {
	DEBUG_ENTER("yield_blocking");

	if (m_state == IEndpoint::Shutdown) {
		return 0;
	}

	/*
	int32_t timeout;
	if (m_transport->outstanding() ||
			(
				m_pending_time_cb.size() == 0
			)) {
		timeout = 1000;
	} else {
		timeout = 0;
	}

	int32_t ret = m_transport->poll(timeout);

	if (ret == -1) {
		DEBUG_LEAVE("yield_blocking -1");
		return ret;
	} else {
		if (m_state == IEndpoint::Shutdown) {
			// Need to shut down
			return 0;
		} else if (m_transport->outstanding() ||
				(
					m_pending_time_cb.size() == 0
				)) {
			ret = 1;
		}
		DEBUG_LEAVE("yield_blocking %d,%d", ret, m_transport->outstanding());
		return ret;
	}
	 */
}

intptr_t EndpointMsgTransport::add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) {
	IParamValMap *params = m_transport->mkValMap();
	params->setVal("time", m_transport->mkValIntU(time, 64));
	intptr_t callback_id = m_callback_id;
	params->setVal("callback-id", m_transport->mkValIntU(callback_id, 32));
	m_callback_id += 1;

	m_callback_m.insert({callback_id, cb_f});

	intptr_t id = m_transport->send_req("tblink.add-time-callback", params);

	rsp_t rsp = wait_rsp(id);

	if (rsp.first) {
		return callback_id;
	} else {
		fprintf(stdout, "Error: no response\n");
		// Error:
		return -1;
	}
}

uint64_t EndpointMsgTransport::time() {
	return m_time;
}

void EndpointMsgTransport::cancel_callback(intptr_t	callback_id) {
	IParamValMap *params = m_transport->mkValMap();

	params->setVal("callback-id", m_transport->mkValIntU(callback_id, 64));

	intptr_t id = m_transport->send_req("tblink.cancel-callback", params);
	rsp_t rsp = wait_rsp(id);
}

/** Called by the environment to notify that
 *  a callback has occurred
 */
void EndpointMsgTransport::notify_callback(intptr_t   callback_id) {
	IParamValMap *params = m_transport->mkValMap();

	params->setVal("callback-id", m_transport->mkValIntU(callback_id, 64));
	params->setVal("time", m_transport->mkValIntU(m_services->time(), 64));

	intptr_t id = m_transport->send_req("tblink.notify-callback", params);

	// TODO: We don't really care about the response directly. Can
	// we let the system know to expect a response, but clear it out
	// once received?
	rsp_t rsp = wait_rsp(id);

	m_services->hit_event();
	m_services->idle();
}

IInterfaceType *EndpointMsgTransport::findInterfaceType(
			const std::string		&name) {
	std::map<std::string,InterfaceType *>::const_iterator it;

	if ((it=m_local_ifc_types.find(name)) != m_local_ifc_types.end()) {
		return it->second;
	} else {
		return 0;
	}
}

IInterfaceTypeBuilder *EndpointMsgTransport::newInterfaceTypeBuilder(
			const std::string		&name) {
	return new InterfaceTypeBuilder(name);
}

IInterfaceType *EndpointMsgTransport::defineInterfaceType(
			IInterfaceTypeBuilder	*type) {
	InterfaceTypeBuilder *builder =
			static_cast<InterfaceTypeBuilder *>(type);
	InterfaceType *if_type = builder->type();
	m_local_ifc_types.insert({if_type->name(), if_type});
	m_local_ifc_type_l.push_back(InterfaceTypeUP(if_type));
	m_local_ifc_type_pl.push_back(if_type);

	return if_type;
}

IInterfaceInst *EndpointMsgTransport::defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			const invoke_req_f		&req_f) {

	InterfaceInst *ifinst = new InterfaceInst(
			this,
			static_cast<InterfaceType *>(type),
			inst_name,
			req_f);
	m_local_ifc_insts.insert({inst_name, ifinst});
	m_local_ifc_insts_l.push_back(JsonInterfaceInstUP(ifinst));
	m_local_ifc_insts_pl.push_back(ifinst);

	return ifinst;
}

int32_t EndpointMsgTransport::process_one_message() {
	return m_transport->process_one_message();
}

const std::vector<IInterfaceType *> &EndpointMsgTransport::getInterfaceTypes() {
	return m_local_ifc_type_pl;
}

const std::vector<IInterfaceInst *> &EndpointMsgTransport::getInterfaceInsts() {
	return m_local_ifc_insts_pl;
}

IParamValBool *EndpointMsgTransport::mkValBool(bool val) {
	return m_transport->mkValBool(val);
}

IParamValInt *EndpointMsgTransport::mkValIntU(uint64_t val, int32_t width) {
	return m_transport->mkValIntU(val, width);
}

IParamValInt *EndpointMsgTransport::mkValIntS(int64_t val, int32_t width) {
	return m_transport->mkValIntS(val, width);
}

IParamValMap *EndpointMsgTransport::mkValMap() {
	return m_transport->mkValMap();
}

IParamValStr *EndpointMsgTransport::mkValStr(const std::string &val) {
	return m_transport->mkValStr(val);
}

IParamValVec *EndpointMsgTransport::mkValVec() {
	return m_transport->mkValVec();
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_build_complete(
		intptr_t				id,
		IParamValMap 			*params) {
	m_build_complete = true;
	IParamValMap *result = 0;
	IParamValMap *error = 0;
	std::vector<InterfaceTypeUP> iftypes = unpack_iftypes(
			params->getValT<IParamValMap>("iftypes"));

	// Process iftypes first
	for (std::vector<InterfaceTypeUP>::iterator
			it=iftypes.begin();
			it!=iftypes.end(); it++) {
		std::map<std::string,InterfaceType*>::const_iterator m_it;
		if ((m_it=m_local_ifc_types.find((*it)->name())) != m_local_ifc_types.end()) {
			// Need to compare
		} else {
			// Add ourselves
			InterfaceType *iftype = it->release();
			m_local_ifc_types.insert({
				iftype->name(),
				iftype
			});
			m_local_ifc_type_l.push_back(InterfaceTypeUP(iftype));
			m_local_ifc_type_pl.push_back(iftype);
		}
	}

	// Now, unpack ifinsts. We need to have a consistent
	// type system prior to doing so
	std::vector<JsonInterfaceInstUP> ifinsts = unpack_ifinsts(
			params->getValT<IParamValMap>("ifinsts"));

	for (std::vector<JsonInterfaceInstUP>::iterator
			it=ifinsts.begin();
			it!=ifinsts.end(); it++) {
		std::map<std::string,InterfaceInst*>::const_iterator m_it;
		if ((m_it=m_local_ifc_insts.find((*it)->name())) != m_local_ifc_insts.end()) {
			// Need to compare
		} else {
			// Add
			InterfaceInst *ifinst = it->release();
			m_local_ifc_insts.insert({
				ifinst->name(),
				ifinst
			});
			m_local_ifc_insts_l.push_back(JsonInterfaceInstUP(ifinst));
			m_local_ifc_insts_pl.push_back(ifinst);
		}
	}

	m_time_precision = params->getValT<IParamValInt>("time-precision")->val_s();

	if (!error) {
		result = m_transport->mkValMap();
	}

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_connect_complete(
		intptr_t				id,
		IParamValMap 			*params) {
	m_connect_complete = true;
	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_get_interface_types(
		intptr_t				id,
		IParamValMap 			*params) {
	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_add_time_callback(
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

	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_notify_callback(
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

	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_shutdown(
		intptr_t				id,
		IParamValMap 			*params) {
	// Avoid recursive efforts to shutdown
	m_state = IEndpoint::Shutdown;

	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_invoke_nb(
		intptr_t				id,
		IParamValMap 			*params) {
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	std::string method = params->getValT<IParamValStr>("method")->val();

	std::map<std::string,InterfaceInst*>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {
		IMethodType *method_t = i_it->second->type()->findMethod(method);

		if (method_t) {
			IParamValVec *m_params = params->getValT<IParamValVec>("params");
			i_it->second->invoke_req(
					method_t,
					m_params,
					std::bind(&EndpointMsgTransport::call_completion_nb,
							this,
							id,
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

	// An invoke request is considered an event
	m_event_received = true;

	IParamValMap *result = 0;
	IParamValMap *error = 0;

	// We will handle our own response
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_invoke_b(
		intptr_t				id,
		IParamValMap 			*params) {
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	std::string method = params->getValT<IParamValStr>("method")->val();
	uint64_t call_id = params->getValT<IParamValInt>("call-id")->val_u();

	std::map<std::string,InterfaceInst*>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {
		IMethodType *method_t = i_it->second->type()->findMethod(method);

		if (method_t) {
			IParamValVec *m_params = params->getValT<IParamValVec>("params");
			i_it->second->invoke_req(
					method_t,
					m_params,
					std::bind(&EndpointMsgTransport::call_completion_b,
							this,
							i_it->second,
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

	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	// Send an 'ok' response. We'll separately send a response message
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_invoke_rsp_b(
		intptr_t				id,
		IParamValMap 			*params) {
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	intptr_t call_id = params->getValT<IParamValInt>("call-id")->val_u();
	IParamVal *retval = 0;

	if (params->hasKey("return")) {
		retval = params->getVal("return");
	}

	std::map<std::string,InterfaceInst*>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {

		i_it->second->invoke_rsp(
				call_id,
				retval);
	} else {
		// TODO: error signaling
		fprintf(stdout, "Error: failed to find interface\n");
		fflush(stdout);
	}

	// A blocking invoke response counts as an event
	m_event_received = true;

	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	// Send an 'ok' response.
	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::req_run_until_event(
		intptr_t				id,
		IParamValMap 			*params) {

	// Tell the environment that it's free to run
	m_services->run_until_event();

	IParamValMap *result = m_transport->mkValMap();
	IParamValMap *error = 0;

	return std::make_pair(IParamValMapUP(result), IParamValMapUP(error));
}

void EndpointMsgTransport::call_completion_nb(
			intptr_t		id,
			IParamVal		*retval) {
	IParamValMap *result = m_transport->mkValMap();
	if (retval) {
		result->setVal("return", retval);
	}
	IParamValMap *error = 0;
	m_transport->send_rsp(id, result, error);
}

void EndpointMsgTransport::call_completion_b(
		InterfaceInst		*ifinst,
		intptr_t				call_id,
		IParamVal				*retval) {
	IParamValMap *params = m_transport->mkValMap();

	params->setVal("ifinst", m_transport->mkValStr(ifinst->name()));
	params->setVal("call-id", m_transport->mkValIntU(call_id, 64));

	if (retval) {
		params->setVal("return", retval);
	}

	intptr_t id = m_transport->send_req("tblink.invoke-rsp-b",
			params);

	wait_rsp(id);

	// Let the services know that we've encountered a
	// time-based event
	m_services->hit_event();

	// Give the endpoint services a chance to decide how
	// best to proceed
	m_services->idle();
}

int32_t EndpointMsgTransport::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMap			*params) {
	DEBUG_ENTER("recv_req: ") << method << " id=" << id;

	IEndpoint::State prev_state = m_state;

	if (id != -1) {
		std::map<std::string,req_func_t>::const_iterator it;

		if ((it=m_req_m.find(method)) != m_req_m.end()) {
			EndpointMsgTransport::rsp_t rsp = it->second(id, params);

			// Note: if neither result not error are non-null,
			// then the method is handling its own response
			if (rsp.first || rsp.second) {
				m_transport->send_rsp(
					id,
					rsp.first.release(),
					rsp.second.release());
			}

			if (m_state == IEndpoint::Shutdown && prev_state != IEndpoint::Shutdown) {
				m_services->shutdown();
			}
		} else {
			fprintf(stdout, "Error: unhandled message %s\n", method.c_str());
			fflush(stdout);
		}
	} else {
		// Notifies
	}

	DEBUG_LEAVE("recv_req: ") << method;

	return 0;
}

int32_t EndpointMsgTransport::recv_rsp(
		intptr_t				id,
		IParamValMap			*result,
		IParamValMap			*error) {
	DEBUG_ENTER("recv_rsp");

	// We take ownership of result and error from the transport
	std::map<intptr_t,rspq_elem_t>::iterator it;

	if ((it=m_rsp_m.find(id)) != m_rsp_m.end()) {
		// Someone will be back to retrieve this ack
		if (it->second.first) {
			it->second.second.first = result;
			it->second.second.first = error;
		} else {
			// The ack is being ignored
			if (result) {
				delete result;
			}
			if (error) {
				delete error;
			}
			m_rsp_m.erase(it);
		}
	}

	DEBUG_LEAVE("recv_rsp");
	return 0;
}

intptr_t EndpointMsgTransport::send_req(
		const std::string 	&method,
		IParamValMap		*params,
		bool				active_wait) {
	intptr_t id = m_transport->send_req(method, params);

	// Insert a placeholder for the response we will receive
	m_rsp_m.insert({id, {active_wait, {0, 0}}});

	return id;
}

EndpointMsgTransport::rsp_t EndpointMsgTransport::wait_rsp(intptr_t id) {
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

	if (it != m_rsp_m.end()) {
		rsp = it->second.second;
		m_rsp_m.erase(it);
		DEBUG_LEAVE("wait_rsp");
		return std::make_pair(IParamValMapUP(rsp.first), IParamValMapUP(rsp.second));
	} else {
		DEBUG_LEAVE("wait_rsp");
		return std::make_pair(IParamValMapUP(), IParamValMapUP());
	}
}

IParamVal *EndpointMsgTransport::invoke_nb(
		InterfaceInst		*ifinst,
		IMethodType 			*method,
		IParamValVec 		*params) {
	IParamValMap *r_params = mkValMap();
	r_params->setVal("ifinst", mkValStr(ifinst->name()));
	r_params->setVal("method", mkValStr(method->name()));
	r_params->setVal("params", params);
	intptr_t id = send_req(
			"tblink.invoke-nb",
			r_params);

	std::pair<IParamValMapSP,IParamValMapSP> rsp = wait_rsp(id);

	// Notify services that there has been an event,
	// and let services determine how best to handle
	m_services->hit_event();
	m_services->idle();

	if (rsp.first->hasKey("return")) {
		// TODO: clone?
		return rsp.first->getVal("return");
	} else {
		return 0;
	}
}

IParamValMap *EndpointMsgTransport::pack_iftypes() {
	IParamValMap *ret = m_transport->mkValMap();

	for (std::map<std::string,InterfaceType*>::const_iterator
			it=m_local_ifc_types.begin();
			it!=m_local_ifc_types.end(); it++) {
		IParamValMap *iftype = m_transport->mkValMap();
		iftype->setVal("name", m_transport->mkValStr(it->second->name()));
		IParamValMap *methods = m_transport->mkValMap();
		for (std::vector<IMethodType *>::const_iterator
				m_it=it->second->methods().begin();
				m_it!=it->second->methods().end(); m_it++) {
			IParamValMap *method = m_transport->mkValMap();
			// TODO:
//			method->setVal("signature", m_transport->mkValStr((*m_it)->signature()));
			method->setVal("is-export", m_transport->mkValBool((*m_it)->is_export()));
			method->setVal("is-blocking", m_transport->mkValBool((*m_it)->is_blocking()));

			methods->setVal((*m_it)->name(), method);
		}
		iftype->setVal("methods", methods);

		ret->setVal(it->second->name(), iftype);
	}

	return ret;
}

IParamValMap *EndpointMsgTransport::pack_ifinsts() {
	IParamValMap *ret = m_transport->mkValMap();

	for (std::map<std::string,InterfaceInst*>::const_iterator
			it=m_local_ifc_insts.begin();
			it!=m_local_ifc_insts.end(); it++) {
		IParamValMap *ifinst = m_transport->mkValMap();
		ifinst->setVal("name", m_transport->mkValStr(it->second->name()));
		ifinst->setVal("type", m_transport->mkValStr(it->second->type()->name()));
		ret->setVal(it->second->name(), ifinst);
	}

	return ret;
}

std::vector<InterfaceTypeUP> EndpointMsgTransport::unpack_iftypes(
		IParamValMap *iftypes) {
	std::vector<InterfaceTypeUP> ret;

	for (std::set<std::string>::const_iterator
			k_it=iftypes->keys().begin();
			k_it!=iftypes->keys().end(); k_it++) {
		IParamValMap *iftype_v = iftypes->getValT<IParamValMap>(*k_it);
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
			iftype->addMethod(method);
		}

		ret.push_back(InterfaceTypeUP(iftype));
	}

	return ret;
}

std::vector<JsonInterfaceInstUP> EndpointMsgTransport::unpack_ifinsts(
		IParamValMap *ifinsts) {
	std::vector<JsonInterfaceInstUP> ret;

	for (std::set<std::string>::const_iterator
			k_it=ifinsts->keys().begin();
			k_it!=ifinsts->keys().end(); k_it++) {
		IParamValMap *ifinst_v = ifinsts->getValT<IParamValMap>(*k_it);
		std::map<std::string,InterfaceType*>::const_iterator it;
		std::string tname = ifinst_v->getValT<IParamValStr>("type")->val();
		InterfaceType *type = 0;
		if ((it=m_local_ifc_types.find(tname)) != m_local_ifc_types.end()) {
			type = it->second;
		}
		InterfaceInst *ifinst = new InterfaceInst(
				this,
				type,
				*k_it);
		ret.push_back(JsonInterfaceInstUP(ifinst));
	}

	return ret;
}

} /* namespace tblink_rpc_core */
