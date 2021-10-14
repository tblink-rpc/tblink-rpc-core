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
#include "tblink_rpc/ILaunchParams.h"
#include "tblink_rpc/ILaunchType.h"
#include "tblink_rpc/ISymFinder.h"
#include "tblink_rpc/ITransport.h"

namespace tblink_rpc_core {

class ITbLink {
public:

	virtual ~ITbLink() { }

	virtual IEndpoint *mkJsonRpcEndpoint(
			IEndpoint::Type		type,
			IEndpointServices	*services) = 0;

	virtual ITransport *mkSocketTransport(
			pid_t			pid,
			int32_t 		socket) = 0;

	virtual const std::vector<ILaunchType *> &launchTypes() const = 0;

	virtual void addLaunchType(ILaunchType *launch_t) = 0;

	virtual ILaunchType *findLaunchType(const std::string &id) = 0;

	virtual ILaunchParams *newLaunchParams() = 0;

	virtual const std::string &getLibPath() = 0;

	virtual ISymFinder *sym_finder() = 0;

};

}
