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
	return new EndpointMsgTransport(type, services);
}

ITransport *Factory::mkSocketTransport(
		pid_t			pid,
		int32_t 		fd) {
	return new TransportJsonSocket(pid, fd);
}

static Factory prvFactory;

} /* namespace tblink_rpc_core */

extern "C" {
tblink_rpc_core::IFactory *tblink_rpc_get_factory() {
	return &tblink_rpc_core::prvFactory;
}
}


