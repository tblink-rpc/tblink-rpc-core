/*
 * LaunchTypeConnectNativeLoopback.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#include "EndpointLoopback.h"
#include "LaunchParams.h"
#include "LaunchTypeConnectNativeLoopback.h"
#include "LaunchTypeRegistration.h"
#include "TbLink.h"

namespace tblink_rpc_core {

LaunchTypeConnectNativeLoopback::LaunchTypeConnectNativeLoopback() {
	// TODO Auto-generated constructor stub

}

LaunchTypeConnectNativeLoopback::~LaunchTypeConnectNativeLoopback() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypeConnectNativeLoopback::launch(ILaunchParams *params) {
	IEndpoint *ep = 0;
	std::string msg;

	ITbLink *tblink = TbLink::inst();

	if (!tblink->getDefaultEP()) {
		msg = "No default endpoint registered";
	} else {
		ep = static_cast<EndpointLoopback *>(tblink->getDefaultEP())->peer();
	}

	delete params;

	return {ep, msg};
}

ILaunchParams *LaunchTypeConnectNativeLoopback::newLaunchParams() {
	return new LaunchParams();
}

static LaunchTypeRegistration<LaunchTypeConnectNativeLoopback> registration;

} /* namespace tblink_rpc_core */
