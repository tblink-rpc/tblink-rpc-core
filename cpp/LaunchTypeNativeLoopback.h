/*
 * LaunchTypeNativeLoopback.h
 *
 *  Created on: Nov 27, 2021
 *      Author: mballance
 */

#pragma once
#include "tblink_rpc/ILaunchType.h"

namespace tblink_rpc_core {

class LaunchTypeNativeLoopback : public ILaunchType {
public:
	LaunchTypeNativeLoopback();

	virtual ~LaunchTypeNativeLoopback();

	virtual std::string name() override { return "native.loopback"; }

	virtual result_t launch(ILaunchParams *params) override;

	virtual ILaunchParams *newLaunchParams() override;

};

} /* namespace tblink_rpc_core */

