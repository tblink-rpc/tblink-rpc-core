/*
 * LaunchTypeConnectNativeLoopback.cpp
 *
 *  Created on: Nov 28, 2021
 *      Author: mballance
 */

#include "tblink_rpc/IEndpointLoopback.h"
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

ILaunchType::result_t LaunchTypeConnectNativeLoopback::launch(
		ILaunchParams 		*params,
		IEndpointServices	*services) {
	IEndpoint *ep = 0;
	std::string msg;

	ITbLink *tblink = TbLink::inst();

	for (auto it=tblink->getEndpoints().begin();
			it!=tblink->getEndpoints().end(); it++) {
		if (((*it)->getFlags() & IEndpointFlags::LoopbackSec) &&
				!((*it)->getFlags() & IEndpointFlags::Claimed)) {
			ep = *it;
			break;
		}
	}

	if (!ep) {
		msg = "No default endpoint registered";
	} else {
		// Claim the endpoint
		ep->setFlag(IEndpointFlags::Claimed);
	}

	delete params;

	return {ep, msg};
}

ILaunchParams *LaunchTypeConnectNativeLoopback::newLaunchParams() {
	return new LaunchParams();
}

static LaunchTypeRegistration<LaunchTypeConnectNativeLoopback> registration;

} /* namespace tblink_rpc_core */
