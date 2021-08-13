/*
 * Factory.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "Factory.h"
#include "JsonRpcEndpoint.h"
#include "SocketMessageTransport.h"

namespace tblink_rpc_core {

Factory::Factory() {
	// TODO Auto-generated constructor stub

}

Factory::~Factory() {
	// TODO Auto-generated destructor stub
}

IEndpoint *Factory::mkJsonRpcEndpoint(
		IEndpointServices	*services) {
	return new JsonRpcEndpoint(services);
}

ITransport *Factory::mkSocketTransport(
		pid_t			pid,
		int32_t 		fd) {
	return new SocketMessageTransport(pid, fd);
}

static Factory prvFactory;

} /* namespace tblink_rpc_core */

extern "C" {
tblink_rpc_core::IFactory *tblink_rpc_get_factory() {
	return &tblink_rpc_core::prvFactory;
}
}


