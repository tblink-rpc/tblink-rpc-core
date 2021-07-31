/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include "JsonRpcEndpoint.h"
#include "JsonInterfaceType.h"
#include "JsonInterfaceTypeBuilder.h"
#include "JsonMethodType.h"
#include "glog/logging.h"

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

JsonRpcEndpoint::JsonRpcEndpoint(
		ITransport 				*transport,
		IEndpointServices		*services) {
	m_transport = transport;
	m_services  = services;

	m_state = IEndpoint::Init;
	m_time = 0;
	m_time_precision = 0;
	m_await_event = false;
	m_event_received = 0;

	m_transport->init(
			std::bind(
					&JsonRpcEndpoint::recv_req,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3),
			std::bind(
					&JsonRpcEndpoint::recv_rsp,
					this,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3)
					);
	m_services->init(this);

	m_build_complete = false;
	m_connect_complete = false;
	m_callback_id = 0;

	m_req_m.insert({"tblink.build-complete", std::bind(
			&JsonRpcEndpoint::req_build_complete, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.connect-complete", std::bind(
			&JsonRpcEndpoint::req_connect_complete, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.add-time-callback", std::bind(
			&JsonRpcEndpoint::req_add_time_callback, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.shutdown", std::bind(
			&JsonRpcEndpoint::req_shutdown, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.notify-callback", std::bind(
			&JsonRpcEndpoint::req_notify_callback, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.invoke-nb", std::bind(
			&JsonRpcEndpoint::req_invoke_nb, this,
			std::placeholders::_1,
			std::placeholders::_2)});
	m_req_m.insert({"tblink.run-until-event", std::bind(
			&JsonRpcEndpoint::req_run_until_event, this,
			std::placeholders::_1,
			std::placeholders::_2)});
}

JsonRpcEndpoint::~JsonRpcEndpoint() {
	// TODO Auto-generated destructor stub
}

bool JsonRpcEndpoint::build_complete() {
	IParamValMapSP params = m_transport->mkValMap();

	// Pack locally-registered interface types and instances
	params->setVal("iftypes", pack_iftypes());
	params->setVal("ifinsts", pack_ifinsts());
	params->setVal("time-precision", m_transport->mkValIntS(
			m_services->time_precision()));

	intptr_t id = m_transport->send_req("tblink.build-complete", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

	while (!m_build_complete) {
		int ret = m_transport->poll(1000);

		if (ret == -1) {
			return false;
		}
	}

	if (m_state != IEndpoint::Shutdown) {
		m_state = IEndpoint::Built;
	}

	return true;
}

bool JsonRpcEndpoint::connect_complete() {
	IParamValMapSP params = m_transport->mkValMap();

	intptr_t id = m_transport->send_req("tblink.connect-complete", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

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
}

bool JsonRpcEndpoint::shutdown() {
	if (m_state != IEndpoint::Shutdown) {
		IParamValMapSP params = m_transport->mkValMap();

		intptr_t id = m_transport->send_req("tblink.shutdown", params);

		// TODO: only wait a little bit for a response...
		//	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);

		m_transport->shutdown();

		m_state = IEndpoint::Shutdown;
	}

	return true;
}

int32_t JsonRpcEndpoint::run_until_event() {
	int32_t ret = 0;

	m_event_received = 0;

	// Send a run request to the peer
	IParamValMapSP params = m_transport->mkValMap();
	intptr_t id = m_transport->send_req(
			"tblink.run-until-event",
			params);

	// Process messages until we receive an event notification
	while (m_event_received == 0) {
		ret = m_transport->await_msg();

		std::map<intptr_t,rsp_t>::const_iterator it;
		if (id > 0 && (it=m_rsp_m.find(id)) != m_rsp_m.end()) {
			// Clear out the response
			m_rsp_m.erase(it);
			id = -1;
		}

		if (ret < 0) {
			break;
		}
	}

	return ret;
}

int32_t JsonRpcEndpoint::await_req() {
	int32_t ret = 0;

	while ((ret=m_transport->await_msg()) == 0) {
		;
	}

	return ret;
}

int32_t JsonRpcEndpoint::yield() {
	DEBUG_ENTER("yield");

	// If we have outstanding messages, or we have
	// no pending time-consuming operations, then
	// wait for longer. Otherwise, just check if
	// there is data.

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
}

int32_t JsonRpcEndpoint::yield_blocking() {
	DEBUG_ENTER("yield_blocking");

	if (m_state == IEndpoint::Shutdown) {
		return 0;
	}

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
}

intptr_t JsonRpcEndpoint::add_time_callback(
			uint64_t						time,
			const std::function<void()>		&cb_f) {
	IParamValMapSP params = m_transport->mkValMap();
	params->setVal("time", m_transport->mkValIntU(time));
	intptr_t callback_id = m_callback_id;
	params->setVal("callback-id", m_transport->mkValIntU(callback_id));
	m_callback_id += 1;

	m_callback_m.insert({callback_id, cb_f});

	intptr_t id = m_transport->send_req("tblink.add-time-callback", params);

	std::pair<IParamValMapSP,IParamValMapSP> rsp = wait_rsp(id);

	if (rsp.first) {
		return callback_id;
	} else {
		fprintf(stdout, "Error: no response\n");
		// Error:
		return -1;
	}
}

uint64_t JsonRpcEndpoint::time() {
	return m_time;
}

void JsonRpcEndpoint::cancel_callback(intptr_t	callback_id) {
	IParamValMapSP params = m_transport->mkValMap();

	params->setVal("callback-id", m_transport->mkValIntU(callback_id));

	intptr_t id = m_transport->send_req("tblink.cancel-callback", params);
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);
}

/** Called by the environment to notify that
 *  a callback has occurred
 */
void JsonRpcEndpoint::notify_callback(intptr_t   callback_id) {
	IParamValMapSP params = m_transport->mkValMap();

	params->setVal("callback-id", m_transport->mkValIntU(callback_id));
	params->setVal("time", m_transport->mkValIntU(m_services->time()));

	intptr_t id = m_transport->send_req("tblink.notify-callback", params);

	// TODO: We don't really care about the response directly. Can
	// we let the system know to expect a response, but clear it out
	// once received?
	std::pair<IParamValSP,IParamValSP> rsp = wait_rsp(id);
}

IInterfaceType *JsonRpcEndpoint::findInterfaceType(
			const std::string		&name) {
	std::map<std::string,IInterfaceTypeUP>::const_iterator it;

	if ((it=m_iftype_m.find(name)) != m_iftype_m.end()) {
		return it->second.get();
	} else {
		return 0;
	}
}

IInterfaceTypeBuilder *JsonRpcEndpoint::newInterfaceTypeBuilder(
			const std::string		&name) {
	return new JsonInterfaceTypeBuilder(name);
}

IInterfaceType *JsonRpcEndpoint::defineInterfaceType(
			IInterfaceTypeBuilder	*type) {
	JsonInterfaceTypeBuilder *builder =
			static_cast<JsonInterfaceTypeBuilder *>(type);
	JsonInterfaceType *if_type = builder->type();
	m_local_ifc_types.insert({if_type->name(), if_type});
	m_local_ifc_type_l.push_back(JsonInterfaceTypeUP(if_type));
	m_local_ifc_type_pl.push_back(if_type);

	return if_type;
}

IInterfaceInst *JsonRpcEndpoint::defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f) {

	JsonInterfaceInst *ifinst = new JsonInterfaceInst(
			this,
			static_cast<JsonInterfaceType *>(type),
			inst_name,
			req_f);
	m_local_ifc_insts.insert({inst_name, ifinst});
	m_local_ifc_insts_l.push_back(JsonInterfaceInstUP(ifinst));
	m_local_ifc_insts_pl.push_back(ifinst);

	return ifinst;
}

const std::vector<IInterfaceType *> &JsonRpcEndpoint::getInterfaceTypes() {
	return m_local_ifc_type_pl;
}

const std::vector<IInterfaceInst *> &JsonRpcEndpoint::getInterfaceInsts() {
	return m_local_ifc_insts_pl;
}

IParamValBoolSP JsonRpcEndpoint::mkValBool(bool val) {
	return m_transport->mkValBool(val);
}

IParamValIntSP JsonRpcEndpoint::mkValIntU(uint64_t val) {
	return m_transport->mkValIntU(val);
}

IParamValIntSP JsonRpcEndpoint::mkValIntS(int64_t val) {
	return m_transport->mkValIntS(val);
}

IParamValMapSP JsonRpcEndpoint::mkValMap() {
	return m_transport->mkValMap();
}

IParamValStrSP JsonRpcEndpoint::mkValStr(const std::string &val) {
	return m_transport->mkValStr(val);
}

IParamValVectorSP JsonRpcEndpoint::mkVector() {
	return m_transport->mkVector();
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_build_complete(
		intptr_t				id,
		IParamValMapSP 			params) {
	m_build_complete = true;
	IParamValMapSP result;
	IParamValMapSP error;
	std::vector<JsonInterfaceTypeUP> iftypes = unpack_iftypes(
			params->getValT<IParamValMap>("iftypes"));

	// Process iftypes first
	for (std::vector<JsonInterfaceTypeUP>::iterator
			it=iftypes.begin();
			it!=iftypes.end(); it++) {
		std::map<std::string,JsonInterfaceType*>::const_iterator m_it;
		if ((m_it=m_local_ifc_types.find((*it)->name())) != m_local_ifc_types.end()) {
			// Need to compare
		} else {
			// Add ourselves
			JsonInterfaceType *iftype = it->release();
			m_local_ifc_types.insert({
				iftype->name(),
				iftype
			});
			m_local_ifc_type_l.push_back(JsonInterfaceTypeUP(iftype));
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
		std::map<std::string,JsonInterfaceInst*>::const_iterator m_it;
		if ((m_it=m_local_ifc_insts.find((*it)->name())) != m_local_ifc_insts.end()) {
			// Need to compare
		} else {
			// Add
			JsonInterfaceInst *ifinst = it->release();
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

	return {result, error};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_connect_complete(
		intptr_t				id,
		IParamValMapSP 			params) {
	m_connect_complete = true;
	IParamValMapSP result = m_transport->mkValMap();
	IParamValMapSP error;

	return {result, error};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_get_interface_types(
		intptr_t				id,
		IParamValMapSP 			params) {
	IParamValMapSP result = m_transport->mkValMap();
	IParamValMapSP error;

	return {result, error};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_add_time_callback(
		intptr_t				id,
		IParamValMapSP 			params) {
	intptr_t callback_id = params->getValT<IParamValInt>("callback-id")->val_u();

	m_services->add_time_cb(
			params->getValT<IParamValInt>("time")->val_s(),
			callback_id);

	// Save the callback id for later
	if (!m_pending_time_cb.insert(callback_id).second) {
		// Error: duplicate ID
	}

	IParamValMapSP result = m_transport->mkValMap();
	IParamValMapSP error;

	return {result, error};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_notify_callback(
		intptr_t				id,
		IParamValMapSP 			params) {
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

	IParamValMapSP result = m_transport->mkValMap();
	IParamValMapSP error;

	return {result, error};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_shutdown(
		intptr_t				id,
		IParamValMapSP 			params) {
	// Avoid recursive efforts to shutdown
	m_state = IEndpoint::Shutdown;

	IParamValMapSP result = m_transport->mkValMap();
	IParamValMapSP error;

	return {result, error};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_invoke_nb(
		intptr_t				id,
		IParamValMapSP 			params) {
	std::string ifinst = params->getValT<IParamValStr>("ifinst")->val();
	std::string method = params->getValT<IParamValStr>("method")->val();

	std::map<std::string,JsonInterfaceInst*>::const_iterator i_it;

	if ((i_it=m_local_ifc_insts.find(ifinst)) != m_local_ifc_insts.end()) {
		IMethodType *method_t = i_it->second->type()->findMethod(method);

		if (method_t) {
			fprintf(stdout, "pre-get-params\n");
			fflush(stdout);
			IParamValVectorSP m_params = params->getValT<IParamValVector>("params");
			fprintf(stdout, "--> invoke_req\n");
			fflush(stdout);
			i_it->second->invoke_req(
					method_t,
					m_params,
					std::bind(&JsonRpcEndpoint::call_completion,
							this,
							id,
							std::placeholders::_1));
			fprintf(stdout, "<-- invoke_req\n");
			fflush(stdout);
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

	IParamValMapSP result;
	IParamValMapSP error;

	// We will handle our own response
	return {0, 0};
}

JsonRpcEndpoint::rsp_t JsonRpcEndpoint::req_run_until_event(
		intptr_t				id,
		IParamValMapSP 			params) {
	m_await_event = true;

	// Tell the environment that it's free to run
	m_services->run_until_event();

	IParamValMapSP result = m_transport->mkValMap();
	IParamValMapSP error;

	return {result, error};
}

void JsonRpcEndpoint::call_completion(
			intptr_t		id,
			IParamValSP		retval) {
	IParamValMapSP result = m_transport->mkValMap();
	fprintf(stdout, "call_completion\n");
	fflush(stdout);
	if (retval) {
		result->setVal("return", retval);
	}
	IParamValMapSP error;
	m_transport->send_rsp(id, result, error);
}

int32_t JsonRpcEndpoint::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMapSP			params) {
	DEBUG_ENTER("recv_req: ") << method << " id=" << id;

	IEndpoint::State prev_state = m_state;

	if (id != -1) {
		std::map<std::string,req_func_t>::const_iterator it;

		if ((it=m_req_m.find(method)) != m_req_m.end()) {
			JsonRpcEndpoint::rsp_t rsp = it->second(id, params);

			// Note: if neither result not error are non-null,
			// then the method is handling its own response
			if (rsp.first || rsp.second) {
				m_transport->send_rsp(
					id,
					rsp.first,
					rsp.second);
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

int32_t JsonRpcEndpoint::recv_rsp(
		intptr_t				id,
		IParamValMapSP			result,
		IParamValMapSP			error) {
	DEBUG_ENTER("recv_rsp");
	m_rsp_m.insert({id, {result, error}});

	DEBUG_LEAVE("recv_rsp");
	return 0;
}

intptr_t JsonRpcEndpoint::send_req(
		const std::string 	&method,
		IParamValMapSP		params) {
	return m_transport->send_req(method, params);
}

std::pair<IParamValMapSP,IParamValMapSP> JsonRpcEndpoint::wait_rsp(intptr_t id) {
	std::map<intptr_t,std::pair<IParamValMapSP,IParamValMapSP>>::iterator it;
	std::pair<IParamValMapSP,IParamValMapSP> rsp;

	DEBUG_ENTER("wait_rsp");

	// Poll waiting
	while ((it=m_rsp_m.find(id)) == m_rsp_m.end()) {

		DEBUG_ENTER("wait_rsp --> await_msg");
		int32_t ret = m_transport->await_msg();
		DEBUG_LEAVE("wait_rsp await_msg ") << ret;

		if (ret == -1) {
			// Failure
			break;
		}
	}

	if (it != m_rsp_m.end()) {
		rsp = it->second;
		m_rsp_m.erase(it);
	}

	DEBUG_LEAVE("wait_rsp");

	return rsp;
}

IParamValMapSP JsonRpcEndpoint::pack_iftypes() {
	IParamValMapSP ret = m_transport->mkValMap();

	for (std::map<std::string,JsonInterfaceType*>::const_iterator
			it=m_local_ifc_types.begin();
			it!=m_local_ifc_types.end(); it++) {
		IParamValMapSP iftype = m_transport->mkValMap();
		iftype->setVal("name", m_transport->mkValStr(it->second->name()));
		IParamValMapSP methods = m_transport->mkValMap();
		for (std::vector<IMethodType *>::const_iterator
				m_it=it->second->methods().begin();
				m_it!=it->second->methods().end(); m_it++) {
			IParamValMapSP method = m_transport->mkValMap();
			method->setVal("signature", m_transport->mkValStr((*m_it)->signature()));
			method->setVal("is-export", m_transport->mkValBool((*m_it)->is_export()));
			method->setVal("is-blocking", m_transport->mkValBool((*m_it)->is_blocking()));

			methods->setVal((*m_it)->name(), method);
		}
		iftype->setVal("methods", methods);

		ret->setVal(it->second->name(), iftype);
	}

	return ret;
}

IParamValMapSP JsonRpcEndpoint::pack_ifinsts() {
	IParamValMapSP ret = m_transport->mkValMap();

	for (std::map<std::string,JsonInterfaceInst*>::const_iterator
			it=m_local_ifc_insts.begin();
			it!=m_local_ifc_insts.end(); it++) {
		IParamValMapSP ifinst = m_transport->mkValMap();
		ifinst->setVal("name", m_transport->mkValStr(it->second->name()));
		ifinst->setVal("type", m_transport->mkValStr(it->second->type()->name()));
		ret->setVal(it->second->name(), ifinst);
	}

	return ret;
}

std::vector<JsonInterfaceTypeUP> JsonRpcEndpoint::unpack_iftypes(
		IParamValMapSP iftypes) {
	std::vector<JsonInterfaceTypeUP> ret;

	for (std::set<std::string>::const_iterator
			k_it=iftypes->keys().begin();
			k_it!=iftypes->keys().end(); k_it++) {
		IParamValMapSP iftype_v = iftypes->getValT<IParamValMap>(*k_it);
		JsonInterfaceType *iftype = new JsonInterfaceType(*k_it);
		IParamValMapSP methods = iftype_v->getValT<IParamValMap>("methods");
		for (std::set<std::string>::const_iterator
				k_it=methods->keys().begin();
				k_it!=methods->keys().end(); k_it++) {
			IParamValMapSP method_v = methods->getValT<IParamValMap>(*k_it);
			JsonMethodType *method = new JsonMethodType(
					(*k_it),
					0, // id
					method_v->getValT<IParamValStr>("signature")->val(),
					// Note: we reverse the direction declared by the peer
					!method_v->getValT<IParamValBool>("is-export")->val(),
					method_v->getValT<IParamValBool>("is-blocking")->val());
			iftype->addMethod(method);
		}

		ret.push_back(JsonInterfaceTypeUP(iftype));
	}

	return ret;
}

std::vector<JsonInterfaceInstUP> JsonRpcEndpoint::unpack_ifinsts(
		IParamValMapSP ifinsts) {
	std::vector<JsonInterfaceInstUP> ret;

	for (std::set<std::string>::const_iterator
			k_it=ifinsts->keys().begin();
			k_it!=ifinsts->keys().end(); k_it++) {
		IParamValMapSP ifinst_v = ifinsts->getValT<IParamValMap>(*k_it);
		std::map<std::string,JsonInterfaceType*>::const_iterator it;
		std::string tname = ifinst_v->getValT<IParamValStr>("type")->val();
		JsonInterfaceType *type = 0;
		if ((it=m_local_ifc_types.find(tname)) != m_local_ifc_types.end()) {
			type = it->second;
		}
		JsonInterfaceInst *ifinst = new JsonInterfaceInst(
				this,
				type,
				*k_it);
		ret.push_back(JsonInterfaceInstUP(ifinst));
	}

	return ret;
}

} /* namespace tblink_rpc_core */
