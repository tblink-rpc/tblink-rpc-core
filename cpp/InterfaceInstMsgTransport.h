/*
 * InterfaceInstMsg.h
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#pragma once
#include "InterfaceInstBase.h"

namespace tblink_rpc_core {

class InterfaceInstMsgTransport;
using InterfaceInstMsgTransportUP=std::unique_ptr<InterfaceInstMsgTransport>;
class InterfaceInstMsgTransport : public InterfaceInstBase {
public:
	InterfaceInstMsgTransport(
			EndpointMsgTransport		*endpoint,
			IInterfaceType				*type,
			const std::string			&inst_name,
			bool						is_mirror);

	InterfaceInstMsgTransport(
			EndpointMsgTransport		*endpoint,
			IInterfaceType				*type,
			const std::string			&inst_name,
			bool						is_mirror,
			const invoke_req_f			&req_f);

	virtual ~InterfaceInstMsgTransport();

	virtual IParamVal *invoke(
			IMethodType									*method,
			IParamValVec								*params) override;

	virtual int32_t invoke_nb(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) override;

};

} /* namespace tblink_rpc_core */

