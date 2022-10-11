/*
 * LaunchTypeConnectSocket.h
 *
 *  Created on: Sep 27, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ILaunchType.h"

namespace tblink_rpc_core {

class LaunchTypeConnectSocket : public ILaunchType {
public:
	LaunchTypeConnectSocket();

	virtual ~LaunchTypeConnectSocket();

	virtual std::string name() override { return "connect.socket"; }

	virtual result_t launch(
			ILaunchParams 			*params,
			IEndpointServices		*services) override;

	virtual ILaunchParams *newLaunchParams() override;

};

} /* namespace tblink_rpc_core */


