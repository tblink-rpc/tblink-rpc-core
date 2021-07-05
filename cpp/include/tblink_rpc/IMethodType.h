/*
 * IMethod.h
 *
 *  Created on: Jul 3, 2021
 *      Author: mballance
 */

#pragma once
#include <memory>

namespace tblink_rpc_core {


class IMethodType {
public:

	virtual ~IMethodType() { }

	virtual const std::string &name() = 0;

	virtual uintptr_t id() = 0;

	virtual const std::string &signature() = 0;


};

}
