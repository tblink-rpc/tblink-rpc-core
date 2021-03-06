/*
 * ILaunchType.h
 *
 *  Created on: Sep 18, 2021
 *      Author: mballance
 */

#pragma once
#include <map>
#include <vector>
#include <string>
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/ILaunchParams.h"

namespace tblink_rpc_core {

class ILaunchType {
public:
	using result_t=std::pair<IEndpoint*,std::string>;

public:
	virtual ~ILaunchType() { }

	virtual std::string name() = 0;

	/**
	 * Launches a peer endpoint and returns the appropriate
	 * endpoint. In the case of an error, the string will
	 * contain an error message
	 *
	 * If the launcher wishes to specify the endpoint services,
	 * then that must be done now. Otherwise, the environment
	 * implementing the launcher will specify the default
	 * services for that environment
	 */
	virtual result_t launch(
			ILaunchParams 		*params,
			IEndpointServices	*services) = 0;

	/**
	 *
	 */
	virtual ILaunchParams *newLaunchParams() = 0;

};

}
