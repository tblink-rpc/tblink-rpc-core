/*
 * IInterfaceInstFactory.h
 *
 *  Created on: Jan 15, 2022
 *      Author: mballance
 */

#pragma once
#include <memory>
#include <string>

namespace tblink_rpc_core {

class IEndpoint;
class IInterfaceImpl;
class IInterfaceType;

class IInterfaceImplFactory;
using IInterfaceImplFactoryUP=std::unique_ptr<IInterfaceImplFactory>;

class IInterfaceImplFactory {
public:

	virtual ~IInterfaceImplFactory() { }

	virtual IInterfaceImpl *createImpl() = 0;

};

}

