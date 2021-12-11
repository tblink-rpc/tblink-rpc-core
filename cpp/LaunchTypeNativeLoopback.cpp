/*
 * LaunchTypeNativeLoopback.cpp
 *
 *  Created on: Nov 27, 2021
 *      Author: mballance
 */

#include "EndpointLoopback.h"
#include "LaunchParams.h"
#include "LaunchTypeNativeLoopback.h"
#include "LaunchTypeRegistration.h"
#include "TbLink.h"

namespace tblink_rpc_core {

LaunchTypeNativeLoopback::LaunchTypeNativeLoopback() {
	// TODO Auto-generated constructor stub

}

LaunchTypeNativeLoopback::~LaunchTypeNativeLoopback() {
	// TODO Auto-generated destructor stub
}

ILaunchType::result_t LaunchTypeNativeLoopback::launch(
		ILaunchParams 			*params,
		IEndpointServices		*services) {
	IEndpoint *ep = 0;
	std::string msg;

	ep = new EndpointLoopback();

	std::map<std::string,std::string>::const_iterator it;

	if ((it=params->params().find("is_default")) != params->params().end()) {
		if (it->second == "1") {
			ITbLink *tblink = TbLink::inst();
			tblink->setDefaultEP(ep);
		}
	}

	delete params;

	if (!services) {
		services = TbLink::inst()->getDefaultServicesFactory()->create();
	}

	ep->init(services, 0);

	return {ep, msg};
}

ILaunchParams *LaunchTypeNativeLoopback::newLaunchParams() {
	return new LaunchParams();
}

static LaunchTypeRegistration<LaunchTypeNativeLoopback>	m_registration;

} /* namespace tblink_rpc_core */
