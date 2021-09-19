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
	 */
	virtual result_t launch(
			const std::vector<std::string>				&args,
			const std::map<std::string,std::string>		&params
			) = 0;

};

}
