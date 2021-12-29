/*
 * IEndpointEvent.h
 *
 *  Created on: Dec 6, 2021
 *      Author: mballance
 */

#pragma once

namespace tblink_rpc_core {

class IEndpointEvent {
public:

	enum kind_t {
		Unknown,
		OutInvokeReqB,
		InInvokeRspB,
		InInvokeReqB,
		OutInvokeRspB,
		OutInvokeReqNB,
		InInvokeRspNB,
		InInvokeReqNB,
		OutInvokeRspNB,
		Terminate
	};

	virtual ~IEndpointEvent() { }

	virtual kind_t kind() const = 0;

};

}
