/*
 * InterfaceInstLoopback.cpp
 *
 *  Created on: Nov 29, 2021
 *      Author: mballance
 */

#include "EndpointLoopback.h"
#include "InterfaceInstLoopback.h"

namespace tblink_rpc_core {

InterfaceInstLoopback::InterfaceInstLoopback(
		EndpointLoopback		*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name,
		bool					is_mirror) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror) {

}

InterfaceInstLoopback::InterfaceInstLoopback(
		EndpointLoopback		*endpoint,
		IInterfaceType			*type,
		const std::string		&inst_name,
		bool					is_mirror,
		const invoke_req_f		&req_f) : InterfaceInstBase(
				endpoint, type, inst_name, is_mirror, req_f) {

}

InterfaceInstLoopback::~InterfaceInstLoopback() {
	// TODO Auto-generated destructor stub
}

IParamVal *InterfaceInstLoopback::invoke(
		IMethodType									*method,
		IParamValVec								*params) {

}

int32_t InterfaceInstLoopback::invoke_nb(
		IMethodType									*method,
		IParamValVec								*params,
		const invoke_rsp_f							&completion_f) {

}

} /* namespace tblink_rpc_core */
