/*
 * IEndpointSupport.h
 *
 *  Created on: Jul 7, 2021
 *      Author: mballance
 */

#pragma once
#include <stdint.h>

namespace tblink_rpc_core {

/**
 * Implements support methods used by an endpoint
 */
class IEndpointServices {
public:

	virtual ~IEndpointServices() { }

	virtual void init(IEndpoint *endpoint) = 0;

	/**
	 * Return command-line arguments.
	 */
	virtual std::vector<std::string> args() = 0;

	virtual void shutdown() = 0;

	virtual intptr_t add_time_cb(
			uint64_t 	time,
			intptr_t	callback_id) = 0;

	virtual void cancel_callback(intptr_t id) = 0;

};

}
