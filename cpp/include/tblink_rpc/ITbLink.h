/*
 * IEndpointFactory.h
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#pragma once
#include <vector>
#include "tblink_rpc/IEndpoint.h"
#include "tblink_rpc/IEndpointServices.h"
#include "tblink_rpc/IEndpointServicesFactory.h"
#include "tblink_rpc/ILaunchParams.h"
#include "tblink_rpc/ILaunchType.h"
#include "tblink_rpc/ISymFinder.h"
#include "tblink_rpc/ITbLinkListener.h"

namespace tblink_rpc_core {

class ITbLink {
public:

	virtual ~ITbLink() { }

	virtual IEndpoint *getDefaultEP() const = 0;

	virtual void setDefaultEP(IEndpoint *) = 0;

	virtual IEndpointServicesFactory *getDefaultServicesFactory() = 0;

	virtual void setDefaultServicesFactory(IEndpointServicesFactory *f) = 0;

	virtual void addListener(ITbLinkListener *l) = 0;

	virtual void removeListener(ITbLinkListener *l) = 0;

	virtual void addEndpoint(IEndpoint *ep, bool is_default=false) = 0;

	virtual void removeEndpoint(IEndpoint *ep) = 0;

	virtual const std::vector<ILaunchType *> &launchTypes() const = 0;

	virtual void addLaunchType(ILaunchType *launch_t) = 0;

	virtual ILaunchType *findLaunchType(const std::string &id) = 0;

	virtual const std::string &getLibPath() = 0;

	virtual ISymFinder *sym_finder() = 0;

	virtual ISymFinder::result_t load_library(
			const std::string &path) = 0;

};

}
