/*
 * IEndpointMsg.h
 *
 *  Created on: Dec 8, 2021
 *      Author: mballance
 */

#pragma once
#include <functional>
#include <stdint.h>
#include <string>
#include "tblink_rpc/IParamValMap.h"

namespace tblink_rpc_core {

class IEndpointMsg {
public:
	using response_f = std::function<void (intptr_t, IParamValMap *, IParamValMap *)>;

	virtual ~IEndpointMsg() { }

	virtual intptr_t send_req(
			const std::string 	&method,
			IParamValMap 		*params) = 0;

	virtual intptr_t send_req(
			const std::string 				&method,
			IParamValMap 					*params,
			const response_f				&rsp_f) = 0;

};
}
