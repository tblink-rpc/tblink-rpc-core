/*
 * Factory.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "TbLink.h"

#include "EndpointMsgTransport.h"
#include "LaunchParams.h"
#include "TransportJsonSocket.h"

namespace tblink_rpc_core {

TbLink::TbLink() {
	// TODO Auto-generated constructor stub

}

TbLink::~TbLink() {
	// TODO Auto-generated destructor stub
}

IEndpoint *TbLink::mkJsonRpcEndpoint(
		IEndpoint::Type		type,
		IEndpointServices	*services) {
//	return new EndpointMsgTransport(type, services);
	return 0;
}

ITransport *TbLink::mkSocketTransport(
		pid_t			pid,
		int32_t 		fd) {
	return new TransportJsonSocket(pid, fd);
}

void TbLink::addLaunchType(ILaunchType *launch_t) {
	m_launch_type_m.insert({launch_t->name(), launch_t});
	m_launch_types.push_back(launch_t);
}

ILaunchType *TbLink::findLaunchType(const std::string &id) {
	std::map<std::string,ILaunchType*>::const_iterator it;

	if ((it=m_launch_type_m.find(id)) != m_launch_type_m.end()) {
		return it->second;
	} else {
		return 0;
	}
}

ILaunchParams *TbLink::newLaunchParams() {
	return new LaunchParams();
}

TbLink *TbLink::inst() {
	if (!m_inst) {
		m_inst = new TbLink();
	}
	return m_inst;
}

TbLink *TbLink::m_inst = 0;

} /* namespace tblink_rpc_core */

extern "C" {
tblink_rpc_core::ITbLink *tblink() {
	return tblink_rpc_core::TbLink::inst();
}
}


