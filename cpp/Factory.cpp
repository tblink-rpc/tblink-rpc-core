/*
 * Factory.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "Factory.h"

#include "EndpointMsgTransport.h"
#include "TransportJsonSocket.h"

namespace tblink_rpc_core {

Factory::Factory() {
	// TODO Auto-generated constructor stub

}

Factory::~Factory() {
	// TODO Auto-generated destructor stub
}

IEndpoint *Factory::mkJsonRpcEndpoint(
		IEndpoint::Type		type,
		IEndpointServices	*services) {
//	return new EndpointMsgTransport(type, services);
	return 0;
}

ITransport *Factory::mkSocketTransport(
		pid_t			pid,
		int32_t 		fd) {
	return new TransportJsonSocket(pid, fd);
}

void Factory::addLaunchType(ILaunchType *launch_t) {
	m_launch_type_m.insert({launch_t->name(), launch_t});
	m_launch_types.push_back(launch_t);
}

ILaunchType *Factory::findLaunchType(const std::string &id) {
	std::map<std::string,ILaunchType*>::const_iterator it;

	if ((it=m_launch_type_m.find(id)) != m_launch_type_m.end()) {
		return it->second;
	} else {
		return 0;
	}
}

Factory *Factory::inst() {
	if (!m_inst) {
		m_inst = std::unique_ptr<Factory>(new Factory());
	}
	return m_inst.get();
}

static Factory prvFactory;

std::unique_ptr<Factory> Factory::m_inst;

} /* namespace tblink_rpc_core */

extern "C" {
tblink_rpc_core::IFactory *tblink_rpc_get_factory() {
	return &tblink_rpc_core::prvFactory;
}
}


