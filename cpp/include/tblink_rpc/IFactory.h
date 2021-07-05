/*
 * IEndpointFactory.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/ITransport.h"

namespace tblink_rpc_core {

class IFactory {
public:

	virtual ~IFactory() { }

	virtual IEndpoint *mkJsonRpcEndpoint(ITransport *t) = 0;

	virtual ITransport *mkSocketTransport(int32_t socket) = 0;

};

}
