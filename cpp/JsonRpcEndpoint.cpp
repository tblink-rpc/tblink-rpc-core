/*
 * Endpoint.cpp
 *
 *  Created on: Jul 2, 2021
 *      Author: mballance
 */

#include "JsonRpcEndpoint.h"
#include "JsonInterfaceType.h"
#include "JsonInterfaceTypeBuilder.h"
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

}

JsonRpcEndpoint::~JsonRpcEndpoint() {
	// TODO Auto-generated destructor stub
}

bool JsonRpcEndpoint::build_complete() {
	IParamValMapSP params = m_transport->mkValMap();

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
//		intptr_t callback_id =
//				rsp.first->getValT<IParamValInt>("callback-id")->val_u();

//		fprintf(stdout, "Rsp callback_id=%lld\n", callback_id);
//		fflush(stdout);
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
	m_iftype_m.insert({if_type->name(), IInterfaceTypeUP(if_type)});

	return if_type;
}

IInterfaceInst *JsonRpcEndpoint::defineInterfaceInst(
			IInterfaceType			*type,
			const std::string		&inst_name,
			const invoke_req_f		&req_f) {

}

int32_t JsonRpcEndpoint::recv_req(
		const std::string		&method,
		intptr_t				id,
		IParamValMapSP			params) {
	DEBUG_ENTER("recv_req: ") << method << " id=" << id;

	if (id != -1) {
		if (method == "tblink.build-complete") {
			m_build_complete = true;
			IParamValMapSP result = m_transport->mkValMap();
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);
		} else if (method == "tblink.connect-complete") {
			m_connect_complete = true;
			IParamValMapSP result = m_transport->mkValMap();
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);
		} else if (method == "tblink.add-time-callback") {
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
			m_transport->send_rsp(
					id,
					result,
					error);
		} else if (method == "tblink.shutdown") {
			// TODO: should receive some sort of status

			fprintf(stdout, "Received Shutdown request\n");
			fflush(stdout);

			// Avoid recursive efforts to shutdown
			m_state = IEndpoint::Shutdown;

			IParamValMapSP result = m_transport->mkValMap();
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);

			m_services->shutdown();
		} else if (method == "tblink.notify-callback") {
			intptr_t callback_id = params->getValT<IParamValInt>("callback-id")->val_s();
			m_time = params->getValT<IParamValInt>("time")->val_u();

			fprintf(stdout, "callback_id: %lld\n", callback_id);


			std::map<intptr_t,std::function<void()>>::const_iterator it;

			if ((it=m_callback_m.find(callback_id)) != m_callback_m.end()) {
				it->second();
				m_callback_m.erase(it);
			} else {
				fprintf(stdout, "Error: callback id %lld not present\n", callback_id);
			}

			IParamValMapSP result = m_transport->mkValMap();
			IParamValMapSP error;
			m_transport->send_rsp(
					id,
					result,
					error);
		} else {
			fprintf(stdout, "Error: unhandled message %s\n", method.c_str());
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

std::pair<IParamValMapSP,IParamValMapSP> JsonRpcEndpoint::wait_rsp(intptr_t id) {
	std::map<intptr_t,std::pair<IParamValMapSP,IParamValMapSP>>::iterator it;
	std::pair<IParamValMapSP,IParamValMapSP> rsp;

	DEBUG_ENTER("wait_rsp");

	// Poll waiting
	while ((it=m_rsp_m.find(id)) == m_rsp_m.end()) {

		DEBUG_ENTER("wait_rsp --> poll");
		int32_t ret = m_transport->poll(1000);
		DEBUG_LEAVE("wait_rsp poll ") << ret;

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

} /* namespace tblink_rpc_core */
