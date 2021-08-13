/*
 * IEndpointFactory.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/ITransport.h"

namespace tblink_rpc_core {

class IFactory {
public:

	virtual ~IFactory() { }

	virtual IEndpoint *mkJsonRpcEndpoint(
			IEndpointServices	*services) = 0;

	virtual ITransport *mkSocketTransport(
			pid_t			pid,
			int32_t 		socket) = 0;

};

}
