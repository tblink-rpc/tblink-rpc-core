/*
 * IEndpointServicesFactory.h
 *
 *  Created on: Dec 4, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>

namespace tblink_rpc_core {

class IEndpointServices;

class IEndpointServicesFactory;
using IEndpointServicesFactoryUP=std::unique_ptr<IEndpointServicesFactory>;
class IEndpointServicesFactory {
public:

	virtual ~IEndpointServicesFactory() { }

	virtual IEndpointServices *create() = 0;

};


}
