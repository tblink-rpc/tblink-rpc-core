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

	if (!tblink->getDefaultEP()) {
		msg = "No default endpoint registered";
	} else {
		IEndpointLoopback *lb_ep = dynamic_cast<IEndpointLoopback *>(tblink->getDefaultEP());
		fprintf(stdout, "Default EP: %p\n", tblink->getDefaultEP());
		fprintf(stdout, "lb_ep=%p\n", lb_ep);
		ep = lb_ep->peer();
		fprintf(stdout, "Peer EP: %p\n", ep);
	}

	delete params;

	return {ep, msg};
}

ILaunchParams *LaunchTypeConnectNativeLoopback::newLaunchParams() {
	return new LaunchParams();
}

static LaunchTypeRegistration<LaunchTypeConnectNativeLoopback> registration;

} /* namespace tblink_rpc_core */
