/*
 * IEndpointLoopback.h
 *
 *  Created on: Dec 8, 2021
 *      Author: mballance
 */

#pragma once

namespace tblink_rpc_core {

class IEndpoint;

class IEndpointLoopback {
public:

	virtual ~IEndpointLoopback() { }

	/**
	 * Returns the endpoint peer
	 */
	virtual IEndpoint *peer() = 0;

};

}
