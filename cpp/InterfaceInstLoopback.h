/*
 * InterfaceInstLoopback.h
 *
 *  Created on: Nov 29, 2021
 *      Author: mballance
 */

#pragma once
#include "InterfaceInstBase.h"
#include "tblink_rpc/IInterfaceType.h"

namespace tblink_rpc_core {

class EndpointLoopback;

class InterfaceInstLoopback : public InterfaceInstBase {
public:
	InterfaceInstLoopback(
			EndpointLoopback		*endpoint,
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror);

	InterfaceInstLoopback(
			EndpointLoopback		*endpoint,
			IInterfaceType			*type,
			const std::string		&inst_name,
			bool					is_mirror,
			const invoke_req_f		&req_f);

	virtual ~InterfaceInstLoopback();

	virtual IParamVal *invoke(
			IMethodType									*method,
			IParamValVec								*params) override;

	virtual int32_t invoke_nb(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) override;

};

} /* namespace tblink_rpc_core */

