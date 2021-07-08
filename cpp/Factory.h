/*
 * Factory.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once

#include "tblink_rpc/IFactory.h"

namespace tblink_rpc_core {

class Factory : public IFactory {
public:
	Factory();

	virtual ~Factory();

	virtual IEndpoint *mkJsonRpcEndpoint(
			ITransport 			*transport,
			IEndpointServices	*services) override;

	virtual ITransport *mkSocketTransport(int32_t socket) override;
};

} /* namespace tblink_rpc_core */

