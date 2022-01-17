/*
 * InterfaceInstMsg.h
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#pragma once
#include "InterfaceInstBase.h"

namespace tblink_rpc_core {

class EndpointMsgBase;
class InterfaceInstMsgTransport;
using InterfaceInstMsgTransportUP=std::unique_ptr<InterfaceInstMsgTransport>;
class InterfaceInstMsgTransport : public InterfaceInstBase {
public:
	InterfaceInstMsgTransport(
			EndpointMsgBase				*endpoint,
			IInterfaceType				*type,
			const std::string			&inst_name,
			bool						is_mirror);

	InterfaceInstMsgTransport(
			EndpointMsgBase				*endpoint,
			IInterfaceType				*type,
			const std::string			&inst_name,
			bool						is_mirror,
			IInterfaceImpl				*impl);

	virtual ~InterfaceInstMsgTransport();

	virtual int32_t invoke(
			IMethodType									*method,
			IParamValVec								*params,
			const invoke_rsp_f							&completion_f) override;

};

} /* namespace tblink_rpc_core */

