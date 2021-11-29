/*
 * LaunchTypeConnectNativeLoopback.h
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ILaunchType.h"

namespace tblink_rpc_core {

class LaunchTypeConnectNativeLoopback : public ILaunchType {
public:
	LaunchTypeConnectNativeLoopback();

	virtual ~LaunchTypeConnectNativeLoopback();

	virtual std::string name() override { return "connect.native.loopback"; }

	virtual result_t launch(ILaunchParams *params) override;

	virtual ILaunchParams *newLaunchParams() override;
};

} /* namespace tblink_rpc_core */

